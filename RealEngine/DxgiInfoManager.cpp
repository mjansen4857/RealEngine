#include "DxgiInfoManager.h"
#include "Window.h"
#include "Graphics.h"
#include <dxgidebug.h>
#include <memory>

#pragma comment(lib, "dxguid.lib")

#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HRException(__LINE__, __FILE__, hr)

DxgiInfoManager::DxgiInfoManager() {
	// define function signature of DXGIGetDebugInterface
	typedef HRESULT(WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	// load the dll that contains the function DXGIGetDebugInterface
	const auto hModDxgiDebug = LoadLibraryEx("dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr) {
		throw RE_WINDOW_LAST_EXCEPTION();
	}

	// get address of DXGIGetDebugInterface in dll
	const auto dxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(
		reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));
	if (dxgiGetDebugInterface == nullptr) {
		throw RE_WINDOW_LAST_EXCEPTION();
	}

	HRESULT hr;
	GFX_THROW_NOINFO(dxgiGetDebugInterface(__uuidof(IDXGIInfoQueue), reinterpret_cast<void**>(&dxgiInfoQueue)));
}

DxgiInfoManager::~DxgiInfoManager() {
	if (dxgiInfoQueue != nullptr) {
		dxgiInfoQueue->Release();
	}
}

void DxgiInfoManager::set() noexcept {
	next = dxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::string> DxgiInfoManager::getMessages() const {
	std::vector<std::string> messages;
	const auto end = dxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto i = next; i < end; i++) {
		HRESULT hr;
		SIZE_T messageLength;
		GFX_THROW_NOINFO(dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, nullptr, &messageLength));
		auto bytes = std::make_unique<byte[]>(messageLength);
		auto message = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());
		GFX_THROW_NOINFO(dxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, i, message, &messageLength));
		messages.emplace_back(message->pDescription);
	}
	return messages;
}
