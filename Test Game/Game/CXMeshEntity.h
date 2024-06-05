#pragma once

#include <vector>
#include "CXEntity.h"
#include "..\Math\CXMat4.h"
#include "..\Math\CXRect.h"
#include "..\Math\CXVec3.h"
#include "..\Window\CXWindow.h"

class  CXGame;
class  CXMeshEntity : public  CXEntity
{
public:
	CXMeshEntity();
	virtual ~CXMeshEntity();
	
	void setMesh(const  CXMeshPtr& mesh);
	void addMaterial(const  CXMaterialPtr& material);
	void removeMaterial(ui32 index);
	CXMeshPtr getMesh();
	std::vector<CXMaterialPtr>& getMaterials();
	
private:
	CXMeshPtr m_mesh;
	std::vector<CXMaterialPtr> m_materials;
	friend class  CXGame;
};
