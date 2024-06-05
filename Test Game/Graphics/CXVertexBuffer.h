#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "..\CXPrerequisites.h"

class  CXVertexBuffer
{
public:
	CXVertexBuffer(const CXVertexBufferDesc& desc,CXGraphicsEngine* system);
	ui32 getVerticesListSize();
	ui32 getVertexSize();
	void* getBuffer();
	void* getLayout();
	
private:
	CXVertexBufferDesc m_desc;
	
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;	
	CXGraphicsEngine* m_system = nullptr;
};
