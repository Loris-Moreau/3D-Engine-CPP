#include <d3dcompiler.h>
#include "CXGraphicsEngine.h"
#include "CXShader.h"

CXShader::CXShader(const CXShaderDesc& desc, CXGraphicsEngine* system)
{
	ID3DBlob* blob = nullptr;
	ID3DBlob* error_blob = nullptr;

	if (FAILED(D3DCompileFromFile(desc.vertexShaderFilePath, nullptr, nullptr, desc.vertexShaderEntryPointName, "vs_5_0", 0, 0, &blob, &error_blob)))
	{
		CX3D_ERROR("CXShaderProgram | Vertex Shader : compiled with errors:")
		//CX3D_ERROR("CXShaderProgram | Vertex Shader : " << desc.vertexShaderFilePath << " compiled with errors:" << std::endl << static_cast<char*>(error_blob->GetBufferPointer()))
	}
	if (FAILED(system->m_d3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_vertexShader)))
	{
		CX3D_ERROR("CXShaderProgram | Vertex Shader hasn't been created successfully.")
	}
	
	if (FAILED(D3DCompileFromFile(desc.pixelShaderFilePath, nullptr, nullptr, desc.pixelShaderEntryPointName, "ps_5_0", 0, 0, &blob, &error_blob)))
	{
		CX3D_ERROR("CXShaderProgram | Pixel Shader : compiled with errors:")
		//CX3D_ERROR("CXShaderProgram | Pixel Shader : " << desc.vertexShaderFilePath << " compiled with errors:" << std::endl << static_cast<char*>(error_blob->GetBufferPointer()));
	}
	if (FAILED(system->m_d3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_pixelShader)))
	{
		CX3D_ERROR("CXShaderProgram | Pixel Shader hasn't been created successfully.")
	}
}

void* CXShader::getVertexShader()
{
	return m_vertexShader.Get();
}

void* CXShader::getPixelShader()
{
	return m_pixelShader.Get();
}
