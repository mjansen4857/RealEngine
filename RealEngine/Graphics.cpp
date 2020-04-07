#include "Graphics.h"
#include "DXError.h"
#include <sstream>
#include <d3dcompiler.h>

namespace wrl = Microsoft::WRL;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "D3DCompiler.lib")

#define GFX_EXCEPTION_NOINFO(hr) Graphics::HRException(__LINE__, __FILE__, (hr))
#define GFX_THROW_NOINFO(hrcall) if(FAILED(hr = (hrcall))) throw Graphics::HRException(__LINE__, __FILE__, hr)

#ifndef NDEBUG
#define GFX_EXCEPTION(hr) Graphics::HRException(__LINE__, __FILE__, (hr), infoManager.getMessages())
#define GFX_THROW_INFO(hrcall) infoManager.set(); if(FAILED(hr = (hrcall))) throw GFX_EXCEPTION(hr)
#define GFX_DEVICE_REMOVED_EXCEPTION(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr), infoManager.getMessages())
#define GFX_THROW_INFO_ONLY(call) infoManager.set(); (call); {auto v = infoManager.getMessages(); if(!v.empty()){throw Graphics::InfoException(__LINE__, __FILE__, v);}}
#else
#define GFX_EXCEPTION(hr) Graphics::HRException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPTION(hr) Graphics::DeviceRemovedException(__LINE__, __FILE__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
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

	wrl::ComPtr<ID3D11Resource> backBuffer;
	GFX_THROW_INFO(swap->GetBuffer(0, __uuidof(ID3D11Resource), &backBuffer));
	GFX_THROW_INFO(device->CreateRenderTargetView(backBuffer.Get(), nullptr, &target));
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
	context->ClearRenderTargetView(target.Get(), color);
}

void Graphics::drawTestTriangle() {
	namespace wrl = Microsoft::WRL;
	HRESULT hr;

	struct Vertex {
		float x;
		float y;
	};

	const Vertex verticies[] = {
		{0.0f, 0.5f},
		{0.5f, -0.5f},
		{-0.5f, -0.5f}
	};

	wrl::ComPtr<ID3D11Buffer> vertexBuffer;
	D3D11_BUFFER_DESC bd = {};
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.CPUAccessFlags = 0u;
	bd.MiscFlags = 0u;
	bd.ByteWidth = sizeof(verticies);
	bd.StructureByteStride = sizeof(Vertex);
	D3D11_SUBRESOURCE_DATA sd = {};
	sd.pSysMem = verticies;
	GFX_THROW_INFO(device->CreateBuffer(&bd, &sd, &vertexBuffer));

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	context->IASetVertexBuffers(0u, 1u, vertexBuffer.GetAddressOf(), &stride, &offset);

	wrl::ComPtr<ID3D11PixelShader> pixelShader;
	wrl::ComPtr<ID3DBlob> blob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &blob));
	GFX_THROW_INFO(device->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &pixelShader));

	context->PSSetShader(pixelShader.Get(), nullptr, 0u);

	wrl::ComPtr<ID3D11VertexShader> vertexShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &blob));
	GFX_THROW_INFO(device->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &vertexShader));

	context->VSSetShader(vertexShader.Get(), nullptr, 0u);

	wrl::ComPtr<ID3D11InputLayout> inputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] = {
		{"Position", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	GFX_THROW_INFO(device->CreateInputLayout(
		ied,
		(UINT) std::size(ied),
		blob->GetBufferPointer(),
		blob->GetBufferSize(),
		&inputLayout
	));

	context->IASetInputLayout(inputLayout.Get());

	context->OMSetRenderTargets(1u, target.GetAddressOf(), nullptr);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_VIEWPORT vp;
	vp.Width = 1280;
	vp.Height = 720;
	vp.MinDepth = 0;
	vp.MaxDepth = 1;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	context->RSSetViewports(1u, &vp);

	GFX_THROW_INFO_ONLY(context->Draw((UINT) std::size(verticies), 0u));
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

Graphics::InfoException::InfoException(int line, const char* file, std::vector<std::string> infoMsgs) noexcept : Exception(line, file) {
	for (const auto& m : infoMsgs) {
		info += m;
		info.push_back('\n');
	}

	if (!info.empty()) {
		info.pop_back();
	}
}

const char* Graphics::InfoException::what() const noexcept {
	std::ostringstream oss;
	oss << getType() << std::endl
		<< "\n[Error Info]\n" << getErrorInfo() << std::endl << std::endl;
	oss << getOriginString();
	whatBuffer = oss.str();
	return whatBuffer.c_str();
}

const char* Graphics::InfoException::getType() const noexcept {
	return "RealEngine Graphics Info Exception";
}

std::string Graphics::InfoException::getErrorInfo() const noexcept {
	return info;
}