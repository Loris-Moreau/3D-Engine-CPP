#pragma once
#include "Prerequisites.h"
#include "Resource.h"
#include "Vector3D.h"
#include "Vector2D.h"
#include "VertexMesh.h"
#include <vector>


class  Mesh : public  Resource
{
public:
	Mesh(const wchar_t* full_path, ResourceManager* manager);
	Mesh(VertexMesh* vertex_list_data, unsigned int vertex_list_size, unsigned int* index_list_data, unsigned int index_list_size,
		MaterialSlot* material_slot_list, unsigned int material_slot_list_size, ResourceManager* manager);
	
	const  VertexBufferPtr& getVertexBuffer();
	const  IndexBufferPtr& getIndexBuffer();

	const  MaterialSlot& getMaterialSlot(unsigned int slot);
	size_t getNumMaterialSlots();

	Vector3D getMaxCorner();
	Vector3D getMinCorner();
	
private:
	void computeTangents(
		const  Vector3D& v0, const  Vector3D& v1, const  Vector3D& v2,
		const  Vector2D& t0, const  Vector2D& t1, const  Vector2D& t2,
		Vector3D& tangent, Vector3D& binormal);
	
	VertexBufferPtr m_vertex_buffer;
	IndexBufferPtr m_index_buffer;
	std::vector<MaterialSlot> m_mat_slots;

	Vector3D m_maxCorner;
	Vector3D m_minCorner = Vector3D(10000, 10000, 10000);
	
	friend class  GraphicsEngine;
};
