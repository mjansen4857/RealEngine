#include "Graphics.h"
#include "DXError.h"
#include <sstream>

#pragma comment(lib, "d3d11.lib")

#define GFX_EXCEPTION_NOINFO(hr) Graphics::HRException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HRException(__LINE__, __FILE__, hr)

#ifndef NDEBUG
#define GFX_EXCEPTION(hr) Graphics::HRException(__LINE__, __FILE__, (hr), infoManager.getMessages())
#define GFX_THROW_INFO(hrcall) infoManager.set(); if(FAILED(hr = (hrcall))) throw GFX_EXCEPTION(hr)
#define GFX_DEVICE_REMOVED_EXCEPTION(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr), infoManager.getMessages())
#else
#define GFX_EXCEPTION(hr) Graphics::HRException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPTION(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#endif

Graphics::Graphics(HWND hWindow) {
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferDesc.Width = 0;
	sd.BufferDesc.Height = 0;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 0;
	sd.BufferDesc.RefreshRate.Denominator = 0;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWindow;
	sd.Windowed = TRUE;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	UINT swapCreateFlags = 0u;
	#ifndef NDEBUG
		swapCreateFlags |= D3D11_CREATE_DEVICE_DEBUG;
	#endif

	HRESULT hr;

	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		swapCreateFlags,
		nullptr,
		0,
		D3D11_SDK_VERSION,
		&sd,
		&swap,
		&device,
		nullptr,
		&context
	));

	ID3D11Resource* backBuffer = nullptr;
	GFX_THROW_INFO(swap->GetBuffer(0, __uuidof(ID3D11Resource), reinterpret_cast<void**>(&backBuffer)));
	GFX_THROW_INFO(device->CreateRenderTargetView(backBuffer, nullptr, &target));
	backBuffer->Release();
}

Graphics::~Graphics() {
	if (target != nullptr) {
		target->Release();
	}

	if (context != nullptr) {
		context->Release();
	}

	if (swap != nullptr) {
		swap->Release();
	}

	if (device != nullptr) {
		device->Release();
	}
}

void Graphics::presentFrame() {
	HRESULT hr;
	#ifndef NDEBUG
		infoManager.set();
	#endif
	if (FAILED(hr = swap->Present(1u, 0u))) {
		if (hr == DXGI_ERROR_DEVICE_REMOVED) {
			throw GFX_DEVICE_REMOVED_EXCEPTION(device->GetDeviceRemovedReason());
		} else {
			throw GFX_EXCEPTION(hr);
		}
	}
}

void Graphics::clearBuffer(float red, float green, float blue) noexcept {
	const float color[] = {red, green, blue, 1.0f};
	context->ClearRenderTargetView(target, color);
}

Graphics::HRException::HRException(int line, const char* file, HRESULT hr, std::vector<std::string> infoMsgs) noexcept : Exception(line, file), hr(hr) {
	for (const auto& m : infoMsgs) {
		info += m;
		info.push_back('\n');
	}

	if (!info.empty()) {
		info.pop_back();
	}
}

const char* Graphics::HRException::what() const noexcept {
	std::ostringstream oss;
	oss << getType() << std::endl
		<< "[Error Code] 0x" << std::hex << std::uppercase << getErrorCode()
		<< std::dec << " (" << (unsigned long) getErrorCode() << ")" << std::endl
		<< "[Error String] " << getErrorString() << std::endl
		<< "[Description] " << getErrorDescription() << std::endl;
	if (!info.empty()) {
		oss << "\n[Error Info]\n" << getErrorInfo() << std::endl << std::endl;
	}
	oss << getOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::HRException::getType() const noexcept {
	return "RealEngine Graphics Exception";
}

HRESULT Graphics::HRException::getErrorCode() const noexcept {
	return hr;
}

std::string Graphics::HRException::getErrorString() const noexcept {
	return DXGetErrorString(hr);
}

std::string Graphics::HRException::getErrorDescription() const noexcept {
	char buf[512];
	DXGetErrorDescription(hr, buf, sizeof(buf));
	return buf;
}

std::string Graphics::HRException::getErrorInfo() const noexcept {
	return info;
}

const char* Graphics::DeviceRemovedException::getType() const noexcept {
	return "RealEngine Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}