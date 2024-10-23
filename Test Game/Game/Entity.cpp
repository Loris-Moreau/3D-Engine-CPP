#include "Entity.h"
#include "Game.h"

Entity::Entity()
{
}

Entity::~Entity()
{
}

void Entity::release()
{
	m_game->removeEntity(this);
}

void Entity::processWorldMatrix()
{
	Mat4 temp;

	m_world.setIdentity();

	temp.setIdentity();
	temp.setScale(m_scale);
	m_world *= temp;
	
	temp.setIdentity();
	temp.setRotationX(m_rotation.x);
	m_world *= temp;

	temp.setIdentity();
	temp.setRotationY(m_rotation.y);
	m_world *= temp;

	temp.setIdentity();
	temp.setRotationZ(m_rotation.z);
	m_world *= temp;
	
	temp.setIdentity();
	temp.setTranslation(m_position);
	m_world *= temp;
}

size_t Entity::getId()
{
	return m_id;
}

void Entity::getWorldMatrix(Mat4& world)
{
	world = m_world;
}

void Entity::setPosition(const  Vec3& position)
{
	m_position = position;
	processWorldMatrix();
}

void Entity::setRotation(const  Vec3& rotation)
{
	m_rotation = rotation;
	processWorldMatrix();
}

void Entity::setScale(const  Vec3& scale)
{
	m_scale = scale;
	processWorldMatrix();
}

Vec3 Entity::getPosition()
{
	return m_position;
}

Vec3 Entity::getRotation()
{
	return m_rotation;
}

Vec3 Entity::getScale()
{
	return m_scale;
}

Game* Entity::getGame()
{
	return m_game;
}

MaterialPtr Entity::createMaterial(const wchar_t* path)
{
	return m_game->createMaterial(path);
}

TexturePtr Entity::createTexture(const wchar_t* path)
{
	return m_game->createTexture(path);
}

MeshPtr Entity::createMesh(const wchar_t* path)
{
	return m_game->createMesh(path);
}
