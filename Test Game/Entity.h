#pragma once

#include <vector>

#include "Matrix4x4.h"
#include "Rect.h"
#include "Vector3D.h"

class  Game;

class  Entity
{
public:
	Entity() = default;
	virtual ~Entity() = default;

	
	size_t getId();
	void getWorldMatrix(Matrix4x4& world);
	void setPosition(const  Vector3D& position);
	void setRotation(const  Vector3D& rotation);
	void setScale(const  Vector3D& scale);

	Vector3D getPosition();
	Vector3D getRotation();
	Vector3D getScale();

	Game* getGame();

	MaterialPtr createMaterial(const wchar_t* path);
	TexturePtr createTexture(const wchar_t* path);
	MeshPtr createMesh(const wchar_t* path);
	
	virtual void onCreate() {}
	virtual void onUpdate(float deltaTime) {}
	virtual void release();
	
protected:
	void processWorldMatrix();
	
	Matrix4x4 m_world;

	Vector3D m_position;
	Vector3D m_rotation;
	Vector3D m_scale = Vector3D(1, 1, 1);

	size_t m_id = 0;

	Game* m_game = nullptr;

	friend class  Game;
};
