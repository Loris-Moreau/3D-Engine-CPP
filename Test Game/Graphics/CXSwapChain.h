#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "..\CXPrerequisites.h"
#include "..\Math\CXRect.h"

struct CXSwapChainDesc
{
	void* windowHandle = nullptr;
	CXRect size;
};

class  CXSwapChain
{
public:
	CXSwapChain(const CXSwapChainDesc& desc, CXGraphicsEngine* system);
	void setFullScreen(bool fullscreen, const  CXRect& size);
	void resize(const  CXRect& size);
	bool present(bool vsync);

	void* getRenderTargetView();
	void* getDepthStencilView();

private:
	void reloadBuffers(unsigned int width, unsigned int height);
	
	Microsoft::WRL::ComPtr <IDXGISwapChain> m_swap_chain = nullptr;
	Microsoft::WRL::ComPtr <ID3D11RenderTargetView> m_rtv = nullptr;
	Microsoft::WRL::ComPtr <ID3D11DepthStencilView> m_dsv = nullptr;
	CXGraphicsEngine* m_system = nullptr;
};
