#include <d3dcompiler.h>
#include "GraphicsEngine.h"
#include "Shader.h"

Shader::Shader(const ShaderDesc& desc, GraphicsEngine* system)
{
	ID3DBlob* blob = nullptr;
	ID3DBlob* error_blob = nullptr;

	if (FAILED(D3DCompileFromFile(desc.vertexShaderFilePath, nullptr, nullptr, desc.vertexShaderEntryPointName, "vs_5_0", 0, 0, &blob, &error_blob)))
	{
		ERROR_3D("ShaderProgram | Vertex Shader : compiled with errors:")
		//ERROR_3D("ShaderProgram | Vertex Shader : " << desc.vertexShaderFilePath << " compiled with errors:" << std::endl << static_cast<char*>(error_blob->GetBufferPointer()))
	}
	if (FAILED(system->m_d3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_vertexShader)))
	{
		ERROR_3D("ShaderProgram | Vertex Shader hasn't been created successfully.")
	}
	
	if (FAILED(D3DCompileFromFile(desc.pixelShaderFilePath, nullptr, nullptr, desc.pixelShaderEntryPointName, "ps_5_0", 0, 0, &blob, &error_blob)))
	{
		ERROR_3D("ShaderProgram | Pixel Shader : compiled with errors:")
		//ERROR_3D("ShaderProgram | Pixel Shader : " << desc.vertexShaderFilePath << " compiled with errors:" << std::endl << static_cast<char*>(error_blob->GetBufferPointer()));
	}
	if (FAILED(system->m_d3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_pixelShader)))
	{
		ERROR_3D("ShaderProgram | Pixel Shader hasn't been created successfully.")
	}
}

void* Shader::getVertexShader()
{
	return m_vertexShader.Get();
}

void* Shader::getPixelShader()
{
	return m_pixelShader.Get();
}
