#pragma once

#include "Prerequisites.h"
#include "Vector4D.h"
#include "Rect.h"

#include "Texture2D.h"
#include "SwapChain.h"
#include <vector>

#include <d3d11.h>
#include <wrl.h>

class  GraphicsEngine
{
public:
	GraphicsEngine();
	
	SwapChainPtr createSwapChain(const SwapChainDesc& desc);
	VertexBufferPtr createVertexBuffer(const VertexBufferDesc& desc);
	IndexBufferPtr createIndexBuffer(const IndexBufferDesc& desc);
	ConstantBufferPtr createConstantBuffer(const ConstantBufferDesc& desc);
	ShaderPtr createShader(const ShaderDesc& desc);
	Texture2DPtr createTexture2D(const Texture2DDesc& desc);
	Texture2DPtr createTexture2D(const wchar_t* path);
	
	void clearColor(const  SwapChainPtr& swap_chain, const Vector4D& color);
	void clearDepthStencil(const  SwapChainPtr& swap_chain);

	void clearColor(const  Texture2DPtr& render_target, const Vector4D& color);
	void clearDepthStencil(const  Texture2DPtr& depth_stencil);

	void setRenderTarget(const  Texture2DPtr& render_target, const  Texture2DPtr& depth_stencil);

	void setVertexBuffer(const  VertexBufferPtr& buffer);
	void setIndexBuffer(const  IndexBufferPtr& buffer);


	void drawTriangleList(unsigned int vertex_count, unsigned int start_vertex_index);
	void drawIndexedTriangleList(unsigned int index_count, unsigned int start_vertex_index, unsigned int start_index_location);
	void drawTriangleStrip(unsigned int vertex_count, unsigned int start_vertex_index);

	void setViewportSize(unsigned int width, unsigned int height);
	void setShader(const  ShaderPtr& shader);
	void setConstantBuffer(const  ConstantBufferPtr& buffer);
	void setRasterizerState(bool cull_front);


	void setTexture(const  TexturePtr* texture, unsigned int num_textures);
	void setMaterial(const  MaterialPtr& material);
	void drawMesh(const  MeshPtr& mesh, const std::vector<MaterialPtr>& list_materials);
	
private:
	void compilePrivateShaders();
	void initRasterizerState();

	
	Microsoft::WRL::ComPtr < ID3D11Device> m_d3dDevice;
	Microsoft::WRL::ComPtr < IDXGIDevice> m_dxgiDevice;
	Microsoft::WRL::ComPtr < IDXGIAdapter> m_dxgiAdapter;
	Microsoft::WRL::ComPtr < IDXGIFactory> m_dxgiFactory;
	Microsoft::WRL::ComPtr < ID3D11DeviceContext> m_immContext;
	
	Microsoft::WRL::ComPtr <ID3D11RasterizerState> m_cullFrontState;
	Microsoft::WRL::ComPtr <ID3D11RasterizerState> m_cullBackState;

	unsigned char m_meshLayoutByteCode[1024];
	size_t m_meshLayoutSize = 0;

	
	friend class  SwapChain;
	friend class  VertexBuffer;
	friend class  IndexBuffer;
	friend class  ConstantBuffer;
	friend class  VertexShader;
	friend class  PixelShader;
	friend class  Texture2D;
	friend class  Shader;
};
