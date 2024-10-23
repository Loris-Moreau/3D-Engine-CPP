#pragma once

#include <vector>
#include "..\Prerequisites.h"
#include "..\Math\Vec2.h"
#include "..\Math\Vec3.h"
#include "..\Math\VertexMesh.h"
#include "Resource.h"

class  Mesh : public  Resource
{
public:
	Mesh(const wchar_t* full_path, ResourceManager* manager);
	Mesh(VertexMesh* vertex_list_data, unsigned int vertex_list_size,
		unsigned int* index_list_data, unsigned int index_list_size,
		MaterialSlot* material_slot_list, unsigned int material_slot_list_size, ResourceManager* manager);
	
	const  VertexBufferPtr& getVertexBuffer();
	const  IndexBufferPtr& getIndexBuffer();

	const  MaterialSlot& getMaterialSlot(unsigned int slot);
	size_t getNumMaterialSlots();

	Vec3 getMaxCorner();
	Vec3 getMinCorner();
	
private:
	void computeTangents
	(
		const  Vec3& v0, const  Vec3& v1, const  Vec3& v2,
		const  Vec2& t0, const  Vec2& t1, const  Vec2& t2,
		Vec3& tangent, Vec3& binormal
	);
	
	VertexBufferPtr m_vertex_buffer;
	IndexBufferPtr m_index_buffer;
	std::vector<MaterialSlot> m_mat_slots;
	
	Vec3 m_maxCorner;
	Vec3 m_minCorner = Vec3(10000, 10000, 10000);
	
	friend class  GraphicsEngine;
};
