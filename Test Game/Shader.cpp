#include "Shader.h"

#include <codecvt>
#include <d3dcompiler.h>
#include <locale>

#include "GraphicsEngine.h"

inline std::string WStringToString(const std::wstring& wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
	return converter.to_bytes(wstr);
}

Shader::Shader(const ShaderDesc& desc, GraphicsEngine* system)
{
	ID3DBlob* blob = nullptr;
	ID3DBlob* error_blob = nullptr;

	if (FAILED(D3DCompileFromFile(desc.vertexShaderFilePath, nullptr, nullptr, desc.vertexShaderEntryPointName, "vs_5_0", 0, 0, &blob, &error_blob)))
	{
		std::string filePath = WStringToString(desc.vertexShaderFilePath); // Convert to std::string
		std::string errorMessage = (char*)error_blob->GetBufferPointer(); // Ensure std::string
		My_ERROR("ShaderProgram | Vertex Shader: " + filePath + " compiled with errors: " + errorMessage)
	}

	if (FAILED(system->m_d3dDevice->CreateVertexShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_vertexShader)))
	{
		My_ERROR("ShaderProgram | Vertex Shader hasn't been created successfully.")
	}


	if (FAILED(D3DCompileFromFile(desc.pixelShaderFilePath, nullptr, nullptr, desc.pixelShaderEntryPointName, "ps_5_0", 0, 0, &blob, &error_blob)))
	{
		std::string filePath = WStringToString(desc.vertexShaderFilePath); // Convert to std::string
		std::string errorMessage = (char*)error_blob->GetBufferPointer(); // Ensure std::string
		My_ERROR("ShaderProgram | Pixel Shader:" + filePath + "compiled with errors:" + errorMessage)
	}
	if (FAILED(system->m_d3dDevice->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_pixelShader)))
	{
		My_ERROR("ShaderProgram | Pixel Shader hasn't been created successfully.")
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