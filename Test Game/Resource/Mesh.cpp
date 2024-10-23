//#include "GraphicsManager.h"
#include "..\Game\Game.h"
#include "..\Graphics\GraphicsEngine.h"
#include "Mesh.h"
#include "ResourceManager.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "..\..\vendor\tinyobjloader\include\tiny_obj_loader.h"

#include <codecvt>
#include <filesystem>
#include <locale>

#include "..\Math\VertexMesh.h"

Mesh::Mesh(const wchar_t* full_path, ResourceManager* manager) : Resource(full_path, manager)
{
	tinyobj::attrib_t attribs;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;

	auto inputfile = std::filesystem::path(full_path).string(); //std::wstring_convert<std::codecvt_utf8<wchar_t>>().to_bytes(full_path);

	std::string mtldir = inputfile.substr(0, inputfile.find_last_of("\\/"));
	
	bool res = tinyobj::LoadObj(&attribs, &shapes, &materials, &warn, &err, inputfile.c_str(), mtldir.c_str());

	if (!err.empty()) throw std::runtime_error("DMesh not created successfully");

	if (!res) throw std::runtime_error("DMesh not created successfully");
	
	std::vector<VertexMesh> list_vertices;
	std::vector<unsigned int> list_indices;

	size_t vertex_buffer_size = 0;
	
	for (size_t s = 0; s < shapes.size(); s++)
	{
		vertex_buffer_size += shapes[s].mesh.indices.size();
	}
	
	list_vertices.reserve(vertex_buffer_size);
	list_indices.reserve(vertex_buffer_size);
	
	m_mat_slots.resize(materials.size());

	size_t index_global_offset = 0;

	for (size_t m = 0; m < materials.size(); m++)
	{
		m_mat_slots[m].start_index = index_global_offset;
		m_mat_slots[m].material_id = m;

		for (size_t s = 0; s < shapes.size(); s++)
		{
			size_t index_offset = 0;

			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++)
			{
				if (shapes[s].mesh.material_ids[f] != m) {
					unsigned char num_face_verts = shapes[s].mesh.num_face_vertices[f];
					index_offset += num_face_verts;
					continue;
				}

				unsigned char num_face_verts = shapes[s].mesh.num_face_vertices[f];

				Vec3 vertices_face[3];
				Vec2 texcoords_face[3];

				for (unsigned char v = 0; v < num_face_verts; v++)
				{
					tinyobj::index_t index = shapes[s].mesh.indices[index_offset + v];

					tinyobj::real_t vx = attribs.vertices[(int)index.vertex_index * 3 + 0];
					tinyobj::real_t vy = attribs.vertices[(int)index.vertex_index * 3 + 1];
					tinyobj::real_t vz = -attribs.vertices[(int)index.vertex_index * 3 + 2];

					tinyobj::real_t tx = 0;
					tinyobj::real_t ty = 0;
					if (attribs.texcoords.size())
					{
						tx = attribs.texcoords[(int)index.texcoord_index * 2 + 0];
						ty = 1.0f - attribs.texcoords[(int)index.texcoord_index * 2 + 1];
					}
					vertices_face[v] = Vec3(vx, vy, vz);
					texcoords_face[v] = Vec2(tx, ty);
				}

				Vec3 tangent, binormal;

				computeTangents
				(
					vertices_face[0], vertices_face[1], vertices_face[2],
					texcoords_face[0], texcoords_face[1], texcoords_face[2],
					tangent, binormal
				);
				
				for (unsigned char v = 0; v < num_face_verts; v++)
				{
					tinyobj::index_t index = shapes[s].mesh.indices[index_offset + v];

					tinyobj::real_t vx = attribs.vertices[(int)index.vertex_index * 3 + 0];
					tinyobj::real_t vy = attribs.vertices[(int)index.vertex_index * 3 + 1];
					tinyobj::real_t vz = -attribs.vertices[(int)index.vertex_index * 3 + 2];
					
					m_maxCorner.x = (m_maxCorner.x < vx) ? vx : m_maxCorner.x;
					m_maxCorner.y = (m_maxCorner.y < vy) ? vy : m_maxCorner.y;
					m_maxCorner.z = (m_maxCorner.x < vz) ? vz : m_maxCorner.z;

					m_minCorner.x = (m_minCorner.x > vx) ? vx : m_minCorner.x;
					m_minCorner.y = (m_minCorner.y > vy) ? vy : m_minCorner.y;
					m_minCorner.z = (m_minCorner.x > vz) ? vz : m_minCorner.z;

					tinyobj::real_t tx = 0;
					tinyobj::real_t ty = 0;
					if (attribs.texcoords.size())
					{
						tx = attribs.texcoords[(int)index.texcoord_index * 2 + 0];
						ty = 1.0f - attribs.texcoords[(int)index.texcoord_index * 2 + 1];
					}

					tinyobj::real_t nx = 0;
					tinyobj::real_t ny = 0;
					tinyobj::real_t nz = 0;

					if (attribs.normals.size())
					{
						nx = attribs.normals[(int)index.normal_index * 3 + 0];
						ny = attribs.normals[(int)index.normal_index * 3 + 1];
						nz = -attribs.normals[(int)index.normal_index * 3 + 2];
					}

					Vec3 v_tangent, v_binormal;
					v_binormal = Vec3::cross(Vec3(nx, ny, nz), tangent);
					v_tangent = Vec3::cross(v_binormal, Vec3(nx, ny, nz));
					
					VertexMesh vertex(Vec3(vx, vy, vz), Vec2(tx, ty), Vec3(nx, ny, nz), v_tangent, v_binormal);
					list_vertices.push_back(vertex);

					list_indices.push_back(static_cast<unsigned int>(index_global_offset) + v);
				}

				index_offset += num_face_verts;
				index_global_offset += num_face_verts;
			}
		}

		m_mat_slots[m].num_indices = index_global_offset - m_mat_slots[m].start_index;
	}
	auto engine = m_resManager->getGame()->getGraphicsEngine();

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	m_vertex_buffer = engine->createVertexBuffer({ &list_vertices[0], sizeof(VertexMesh), static_cast<UINT>(list_vertices.size()) });
	m_index_buffer = engine->createIndexBuffer({ &list_indices[0], static_cast<UINT>(list_indices.size()) });
}

Mesh::Mesh(VertexMesh* vertex_list_data, unsigned int vertex_list_size, unsigned int* index_list_data,
	unsigned int index_list_size, MaterialSlot* material_slot_list, unsigned int material_slot_list_size,
	ResourceManager* manager) :
		Resource(L"", manager)
{
	auto engine = m_resManager->getGame()->getGraphicsEngine();

	void* shader_byte_code = nullptr;
	size_t size_shader = 0;
	m_vertex_buffer = engine->createVertexBuffer({ vertex_list_data, sizeof(VertexMesh), (UINT)vertex_list_size });
	m_index_buffer = engine->createIndexBuffer({ index_list_data, (UINT)index_list_size });

	m_mat_slots.resize(material_slot_list_size);

	for (unsigned int i = 0; i < material_slot_list_size; i++)
	{
		m_mat_slots[i] = material_slot_list[i];
	}
}

const  VertexBufferPtr& Mesh::getVertexBuffer()
{
	return m_vertex_buffer;
}

const  IndexBufferPtr& Mesh::getIndexBuffer()
{
	return m_index_buffer;
}

const  MaterialSlot& Mesh::getMaterialSlot(unsigned int slot)
{
	return m_mat_slots[slot];
}

size_t Mesh::getNumMaterialSlots()
{
	return m_mat_slots.size();
}

Vec3 Mesh::getMaxCorner()
{
	return m_maxCorner;
}

Vec3 Mesh::getMinCorner()
{
	return m_minCorner;
}

void Mesh::computeTangents(const  Vec3& v0,
	const  Vec3& v1,
	const  Vec3& v2,
	const  Vec2& t0,
	const  Vec2& t1,
	const  Vec2& t2,
	Vec3& tangent, Vec3& binormal)
{
	Vec3 deltaPos1 = v1 - v0;
	Vec3 deltaPos2 = v2 - v0;

	Vec2 deltaUV1 = t1 - t0;
	Vec2 deltaUV2 = t2 - t0;
	
	float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
	tangent = (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y);
	tangent = Vec3::normalize(tangent);
	binormal = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x);
	binormal = Vec3::normalize(binormal);
}
