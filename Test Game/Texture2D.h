#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "Prerequisites.h"
#include "Rect.h"

struct Texture2DDesc
{
    Rect size = {};
    TextureType type = TextureType::Normal;
};

class  Texture2D
{
public:
    Texture2D(const Texture2DDesc& desc, GraphicsEngine* system);
    Texture2D(const wchar_t* path, GraphicsEngine* system);
    
private:
    Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture = nullptr;
    Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shader_res_view = nullptr;
    Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler_state = nullptr;
    Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_render_target_view = nullptr;
    Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depth_stencil_view = nullptr;
    
    Texture2DDesc m_desc = {};
    
    friend class  GraphicsEngine;
};
