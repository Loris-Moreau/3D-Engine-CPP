#pragma once

#include <vector>
#include "..\Math\CXMat4.h"
#include "..\Math\CXRect.h"
#include "..\Math\CXVec3.h"

class  CXGame;
class  CXEntity
{
public:
	CXEntity();
	virtual ~CXEntity();
	
	size_t getId();
	void getWorldMatrix(CXMat4& world);
	void setPosition(const  CXVec3& position);
	void setRotation(const  CXVec3& rotation);
	void setScale(const  CXVec3& scale);
	
	CXVec3 getPosition();
	CXVec3 getRotation();
	CXVec3 getScale();

	CXGame* getGame();

	CXMaterialPtr createMaterial(const wchar_t* path);
	CXTexturePtr createTexture(const wchar_t* path);
	CXMeshPtr createMesh(const wchar_t* path);
	
	virtual void onCreate() {}
	virtual void onUpdate(f32 deltaTime) {}
	virtual void release();
	
protected:
	void processWorldMatrix();
	
	CXMat4 m_world;

	CXVec3 m_position;
	CXVec3 m_rotation;
	CXVec3 m_scale = CXVec3(1, 1, 1);

	size_t m_id = 0;

	CXGame* m_game = nullptr;

	friend class  CXGame;
};
