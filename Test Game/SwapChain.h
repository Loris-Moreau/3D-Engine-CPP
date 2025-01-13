#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "Prerequisites.h"
#include "Rect.h"

struct SwapChainDesc
{
	void* windowHandle = nullptr;
	Rect size;
};

class  SwapChain
{
public:
	SwapChain(const SwapChainDesc& desc, GraphicsEngine* system);
	void setFullScreen(bool fullscreen, const  Rect& size);
	void resize(const  Rect& size);
	bool present(bool vsync);

	void* getRenderTargetView();
	void* getDepthStencilView();
	
private:
	void reloadBuffers(unsigned int width, unsigned int height);

public:
	Microsoft::WRL::ComPtr <IDXGISwapChain> m_swap_chain = nullptr;
	Microsoft::WRL::ComPtr <ID3D11RenderTargetView> m_rtv = nullptr;
	Microsoft::WRL::ComPtr <ID3D11DepthStencilView> m_dsv = nullptr;
	
	GraphicsEngine* m_system = nullptr;
};
