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
	
	// Collision radius (sphere-based). Set > 0 to enable collision detection.
	void setCollisionRadius(float radius) { m_collisionRadius = radius; }
	float getCollisionRadius() const { return m_collisionRadius; }
	bool isCollidable() const { return m_collisionRadius > 0.0f; }

	virtual void onCreate() {}
	virtual void onUpdate(float deltaTime) {}
	virtual void release();

	// Set the world matrix directly, bypassing Euler-angle processWorldMatrix.
	// Used by Ship to drive the camera with an orientation matrix.
	void setWorldMatrix(const Matrix4x4& mat)
	{
		m_world = mat;
		m_position = Vector3D(mat.mat[3][0], mat.mat[3][1], mat.mat[3][2]);
	}
	
protected:
	void processWorldMatrix();
	
	Matrix4x4 m_world;

	Vector3D m_position;
	Vector3D m_rotation;
	Vector3D m_scale = Vector3D(1, 1, 1);

	float m_collisionRadius = 0.0f;

	size_t m_id = 0;

	Game* m_game = nullptr;

	friend class  Game;
};
