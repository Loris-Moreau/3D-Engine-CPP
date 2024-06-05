#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "..\CXPrerequisites.h"

class  CXShader
{
public:
	CXShader(const CXShaderDesc& desc, CXGraphicsEngine* system);
	
	void* getVertexShader();
	void* getPixelShader();
	
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
	
	CXGraphicsEngine* m_system = nullptr;
};
