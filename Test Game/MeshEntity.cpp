#include "MeshEntity.h"
#include "Mesh.h"

MeshEntity::MeshEntity()
{
}

MeshEntity::~MeshEntity()
{
}


void MeshEntity::setMesh(const  MeshPtr& mesh)
{
    m_mesh = mesh;
}

void MeshEntity::addMaterial(const  MaterialPtr& material)
{
    m_materials.push_back(material);
}

void MeshEntity::removeMaterial(unsigned int index)
{
    m_materials.erase(std::next(m_materials.begin(), index));
}

MeshPtr MeshEntity::getMesh()
{
    return m_mesh;
}

std::vector<MaterialPtr>& MeshEntity::getMaterials()
{
    return m_materials;
}