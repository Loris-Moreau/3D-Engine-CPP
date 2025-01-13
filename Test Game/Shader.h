#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "Prerequisites.h"

class  Shader
{
public:
    Shader(const ShaderDesc& desc, GraphicsEngine* system);
    
    void* getVertexShader();
    void* getPixelShader();
    
private:
    Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
    Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
    GraphicsEngine* m_system = nullptr;
};
