#pragma once

#include "Vec2.h"
#include "Vec3.h"

class  VertexMesh
{
public:
	VertexMesh() :m_position(), m_texcoord(), m_normal() {}
	
	VertexMesh(const  Vec3& position, const  Vec2& texcoord, const  Vec3& normal,
		const  Vec3& tangent, const  Vec3& binormal) :
		m_position(position),
		m_texcoord(texcoord),
		m_normal(normal),
		m_tangent(tangent),
		m_binormal(binormal)
	{}
	
	VertexMesh(const  VertexMesh& vertex) :
		m_position(vertex.m_position),
		m_texcoord(vertex.m_texcoord),
		m_normal(vertex.m_normal),
		m_tangent(vertex.m_tangent),
		m_binormal(vertex.m_binormal)
	{}
	
	Vec3 m_position;
	Vec2 m_texcoord;
	Vec3 m_normal;
	Vec3 m_tangent;
	Vec3 m_binormal;
};
