#pragma once

#include <d3d11.h>
#include <wrl.h>
#include "..\Prerequisites.h"

class  ConstantBuffer
{
public:
	ConstantBuffer(const ConstantBufferDesc& desc, GraphicsEngine* system);
	
	void update(void* buffer);
	void* getBuffer();
	
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	GraphicsEngine* m_system = nullptr;
};
