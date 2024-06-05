#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "..\CXPrerequisites.h"

class  CXIndexBuffer
{
public:
	CXIndexBuffer(const CXIndexBufferDesc& desc, CXGraphicsEngine* system);
	ui32 getIndicesListSize();
	void* getBuffer();
	
private:
	CXIndexBufferDesc m_desc;
	
	Microsoft::WRL::ComPtr <ID3D11Buffer> m_buffer;
	CXGraphicsEngine* m_system = nullptr;
};
