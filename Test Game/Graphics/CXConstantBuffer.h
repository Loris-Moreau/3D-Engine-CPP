#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "..\CXPrerequisites.h"

class  CXConstantBuffer
{
public:
	CXConstantBuffer(const CXConstantBufferDesc& desc, CXGraphicsEngine* system);
	
	void update(void* buffer);
	void* getBuffer();
	
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	CXGraphicsEngine* m_system = nullptr;
};
