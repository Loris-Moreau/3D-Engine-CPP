#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "..\Prerequisites.h"

class  VertexBuffer
{
public:
	VertexBuffer(const VertexBufferDesc& desc,GraphicsEngine* system);
	ui32 getVerticesListSize();
	ui32 getVertexSize();
	void* getBuffer();
	void* getLayout();
	
private:
	VertexBufferDesc m_desc;
	
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;	
	GraphicsEngine* m_system = nullptr;
};
