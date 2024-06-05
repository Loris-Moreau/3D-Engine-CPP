#include "CXMeshEntity.h"
#include "..\Resource\CXMesh.h"

CXMeshEntity::CXMeshEntity()
{
}

CXMeshEntity::~CXMeshEntity()
{
}

void CXMeshEntity::setMesh(const  CXMeshPtr& mesh)
{
	m_mesh = mesh;
}

void CXMeshEntity::addMaterial(const  CXMaterialPtr& material)
{
	m_materials.push_back(material);
}

void CXMeshEntity::removeMaterial(ui32 index)
{
	m_materials.erase(std::next(m_materials.begin(), index));
}

CXMeshPtr CXMeshEntity::getMesh()
{
	return m_mesh;
}

std::vector<CXMaterialPtr>& CXMeshEntity::getMaterials()
{
	return m_materials;
}
