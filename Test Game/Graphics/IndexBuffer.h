#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "..\Prerequisites.h"

class  IndexBuffer
{
public:
	IndexBuffer(const IndexBufferDesc& desc, GraphicsEngine* system);
	ui32 getIndicesListSize();
	void* getBuffer();
	
private:
	IndexBufferDesc m_desc;
	
	Microsoft::WRL::ComPtr <ID3D11Buffer> m_buffer;
	GraphicsEngine* m_system = nullptr;
};
