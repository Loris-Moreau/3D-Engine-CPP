#include <stdexcept>
#include <string>
#include "GraphicsEngine.h"
#include "SwapChain.h"

SwapChain::SwapChain(const SwapChainDesc& desc, GraphicsEngine* system) : m_system(system)
{
	auto device = m_system->m_d3dDevice;

	DXGI_SWAP_CHAIN_DESC d3d11Desc = {};
	d3d11Desc.BufferCount = 1;
	d3d11Desc.BufferDesc.Width = (desc.size.width > 0) ? desc.size.width : 1;
	d3d11Desc.BufferDesc.Height = (desc.size.height > 0) ? desc.size.height : 1;
	d3d11Desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3d11Desc.BufferDesc.RefreshRate.Numerator = 60;
	d3d11Desc.BufferDesc.RefreshRate.Denominator = 1; 
	d3d11Desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	d3d11Desc.OutputWindow = static_cast<HWND>(desc.windowHandle);
	d3d11Desc.SampleDesc.Count = 1;
	d3d11Desc.SampleDesc.Quality = 0;
	d3d11Desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	d3d11Desc.Windowed = TRUE;
	
	HRESULT hr = m_system->m_dxgiFactory->CreateSwapChain(device.Get(), &d3d11Desc, &m_swap_chain);

	if (FAILED(hr))
	{
		throw std::runtime_error("DSwapChain not created successfully");
	}
	
	reloadBuffers(desc.size.width, desc.size.height);
}

void SwapChain::setFullScreen(bool fullscreen, const  Rect& size)
{
	resize(size);
	m_swap_chain->SetFullscreenState(fullscreen, nullptr);
}

void SwapChain::resize(const  Rect& size)
{
	m_rtv.Reset();
	m_dsv.Reset();
	
	m_swap_chain->ResizeBuffers(1, size.width, size.height, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	reloadBuffers(size.width, size.height);
}

bool SwapChain::present(bool vsync)
{
	m_swap_chain->Present(vsync, NULL);
	return true;
}

void* SwapChain::getRenderTargetView()
{
	return m_rtv.Get();
}

void* SwapChain::getDepthStencilView()
{
	return m_dsv.Get();
}

void SwapChain::reloadBuffers(unsigned int width, unsigned int height)
{
	auto device = m_system->m_d3dDevice;

	//Get the back buffer color and create its render target view
	//-----------------------------------------------------
	ID3D11Texture2D* buffer = nullptr;
	HRESULT hr = m_swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&buffer));

	if (FAILED(hr))
	{
		throw std::runtime_error("DSwapChain not created successfully");
	}
	
	hr = device->CreateRenderTargetView(buffer, nullptr, &m_rtv);
	buffer->Release();

	if (FAILED(hr))
	{
		throw std::runtime_error("DSwapChain not created successfully");
	}
	
	D3D11_TEXTURE2D_DESC tex_desc = {};
	tex_desc.Width = (width > 0) ? width : 1;
	tex_desc.Height = (height > 0) ? height : 1;
	tex_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tex_desc.Usage = D3D11_USAGE_DEFAULT;
	tex_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	tex_desc.MipLevels = 1;
	tex_desc.SampleDesc.Count = 1;
	tex_desc.SampleDesc.Quality = 0;
	tex_desc.MiscFlags = 0;
	tex_desc.ArraySize = 1;
	tex_desc.CPUAccessFlags = 0;
	
	hr = device->CreateTexture2D(&tex_desc, nullptr, &buffer);
	if (FAILED(hr))
	{
		throw std::runtime_error("DSwapChain not created successfully");
	}
	
	hr = device->CreateDepthStencilView(buffer, nullptr, &m_dsv);
	buffer->Release();

	if (FAILED(hr))
	{
		throw std::runtime_error("DSwapChain not created successfully");
	}
}
