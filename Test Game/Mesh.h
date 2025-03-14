#pragma once

#include "IndexBuffer.h"
#include "Resource.h"
#include "VertexBuffer.h"

class Mesh: public Resource
{
public:
	Mesh(const wchar_t* full_path);
	~Mesh();
	
	const VertexBufferPtr& getVertexBuffer();
	const IndexBufferPtr& getIndexBuffer();
	
private:
	VertexBufferPtr m_vertex_buffer;
	IndexBufferPtr m_index_buffer;
	
	friend class DeviceContext;
};
