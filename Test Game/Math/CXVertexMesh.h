#pragma once

#include "CXVec2.h"
#include "CXVec3.h"

class  CXVertexMesh
{
public:
	CXVertexMesh() :m_position(), m_texcoord(), m_normal() {}
	
	CXVertexMesh(const  CXVec3& position, const  CXVec2& texcoord, const  CXVec3& normal,
		const  CXVec3& tangent, const  CXVec3& binormal) :
		m_position(position),
		m_texcoord(texcoord),
		m_normal(normal),
		m_tangent(tangent),
		m_binormal(binormal)
	{}
	
	CXVertexMesh(const  CXVertexMesh& vertex) :
		m_position(vertex.m_position),
		m_texcoord(vertex.m_texcoord),
		m_normal(vertex.m_normal),
		m_tangent(vertex.m_tangent),
		m_binormal(vertex.m_binormal)
	{}
	
	CXVec3 m_position;
	CXVec2 m_texcoord;
	CXVec3 m_normal;
	CXVec3 m_tangent;
	CXVec3 m_binormal;
};
