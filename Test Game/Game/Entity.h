#pragma once

#include <vector>
#include "..\Math\Mat4.h"
#include "..\Math\Rect.h"
#include "..\Math\Vec3.h"

class  Game;
class  Entity
{
public:
	Entity();
	virtual ~Entity();
	
	size_t getId();
	void getWorldMatrix(Mat4& world);
	void setPosition(const  Vec3& position);
	void setRotation(const  Vec3& rotation);
	void setScale(const  Vec3& scale);
	
	Vec3 getPosition();
	Vec3 getRotation();
	Vec3 getScale();

	Game* getGame();

	MaterialPtr createMaterial(const wchar_t* path);
	TexturePtr createTexture(const wchar_t* path);
	MeshPtr createMesh(const wchar_t* path);
	
	virtual void onCreate() {}
	virtual void onUpdate(f32 deltaTime) {}
	virtual void release();
	
protected:
	void processWorldMatrix();
	
	Mat4 m_world;

	Vec3 m_position;
	Vec3 m_rotation;
	Vec3 m_scale = Vec3(1, 1, 1);

	size_t m_id = 0;

	Game* m_game = nullptr;

	friend class  Game;
};
