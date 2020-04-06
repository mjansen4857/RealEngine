#pragma once
#include "RealWin.h"
#include <d3d11.h>

class Graphics {
public:
	Graphics(HWND hWindow);
	Graphics(const Graphics&) = delete;
	Graphics& operator = (const Graphics&) = delete;
	~Graphics();
	void presentFrame();
	void clearBuffer(float red, float green, float blue) noexcept;
private:
	ID3D11Device* device = nullptr;
	IDXGISwapChain* swap = nullptr;
	ID3D11DeviceContext* context = nullptr;
	ID3D11RenderTargetView* target = nullptr;
};

