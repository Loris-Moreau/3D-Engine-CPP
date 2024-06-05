#pragma once

#include <vector>
#include "..\CXPrerequisites.h"
#include "..\Math\CXVec2.h"
#include "..\Math\CXVec3.h"
#include "..\Math\CXVertexMesh.h"
#include "..\Resource\CXResource.h"

class  CXMesh : public  CXResource
{
public:
	CXMesh(const wchar_t* full_path, CXResourceManager* manager);
	CXMesh(CXVertexMesh* vertex_list_data, unsigned int vertex_list_size,
		unsigned int* index_list_data, unsigned int index_list_size,
		CXMaterialSlot* material_slot_list, unsigned int material_slot_list_size, CXResourceManager* manager);
	
	const  CXVertexBufferPtr& getVertexBuffer();
	const  CXIndexBufferPtr& getIndexBuffer();

	const  CXMaterialSlot& getMaterialSlot(unsigned int slot);
	size_t getNumMaterialSlots();

	CXVec3 getMaxCorner();
	CXVec3 getMinCorner();
	
private:
	void computeTangents
	(
		const  CXVec3& v0, const  CXVec3& v1, const  CXVec3& v2,
		const  CXVec2& t0, const  CXVec2& t1, const  CXVec2& t2,
		CXVec3& tangent, CXVec3& binormal
	);
	
	CXVertexBufferPtr m_vertex_buffer;
	CXIndexBufferPtr m_index_buffer;
	std::vector<CXMaterialSlot> m_mat_slots;
	
	CXVec3 m_maxCorner;
	CXVec3 m_minCorner = CXVec3(10000, 10000, 10000);
	
	friend class  CXGraphicsEngine;
};
