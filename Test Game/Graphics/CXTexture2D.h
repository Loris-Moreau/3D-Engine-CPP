#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "..\CXPrerequisites.h"
#include "..\Math\CXRect.h"

struct CXTexture2DDesc
{
	CXRect size = {};
	CXTextureType type = CXTextureType::Normal;
};

class  CXTexture2D
{
public:
	CXTexture2D(const CXTexture2DDesc& desc, CXGraphicsEngine* system);
	CXTexture2D(const wchar_t* path, CXGraphicsEngine* system);
	
private:
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shader_res_view = nullptr;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler_state = nullptr;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_render_target_view = nullptr;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depth_stencil_view = nullptr;
	CXTexture2DDesc m_desc = {};
	
	friend class  CXGraphicsEngine;
};
