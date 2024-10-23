#pragma once

#include <vector>
#include "Entity.h"
#include "..\Math\Mat4.h"
#include "..\Math\Rect.h"
#include "..\Math\Vec3.h"
#include "..\Window\Window.h"

class  Game;
class  MeshEntity : public  Entity
{
public:
	MeshEntity();
	virtual ~MeshEntity();
	
	void setMesh(const  MeshPtr& mesh);
	void addMaterial(const  MaterialPtr& material);
	void removeMaterial(ui32 index);
	MeshPtr getMesh();
	std::vector<MaterialPtr>& getMaterials();
	
private:
	MeshPtr m_mesh;
	std::vector<MaterialPtr> m_materials;
	friend class  Game;
};
