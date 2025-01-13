#pragma once

#include "Entity.h"
#include "Matrix4x4.h"
#include "Rect.h"
#include "Vector3D.h"
#include "Window.h"

#include <vector>

class  Game;

class MeshEntity : public  Entity
{
public:
    MeshEntity();
    virtual ~MeshEntity();
    
    void setMesh(const  MeshPtr& mesh);
    void addMaterial(const  MaterialPtr& material);
    void removeMaterial(unsigned int index);
    MeshPtr getMesh();
    std::vector<MaterialPtr>& getMaterials();
    
private:
    MeshPtr m_mesh;
    std::vector<MaterialPtr> m_materials;
    friend class Game;
};
