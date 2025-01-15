#include "Entity.h"
#include "Game.h"

void Entity::release()
{
	m_game->removeEntity(this);
}

void Entity::processWorldMatrix()
{
	Matrix4x4 temp;

	m_world.setIdentity();

	temp.setIdentity();
	temp.setScale(m_scale);
	m_world *= temp;


	temp.setIdentity();
	temp.setRotationX(m_rotation.m_x);
	m_world *= temp;

	temp.setIdentity();
	temp.setRotationY(m_rotation.m_y);
	m_world *= temp;

	temp.setIdentity();
	temp.setRotationZ(m_rotation.m_z);
	m_world *= temp;


	temp.setIdentity();
	temp.setTranslation(m_position);
	m_world *= temp;
}


size_t Entity::getId()
{
	return m_id;
}

void Entity::getWorldMatrix(Matrix4x4& world)
{
	world = m_world;
}

void Entity::setPosition(const Vector3D& position)
{
	m_position = position;
	processWorldMatrix();
}

void Entity::setRotation(const Vector3D& rotation)
{
	m_rotation = rotation;
	processWorldMatrix();
}

void Entity::setScale(const Vector3D& scale)
{
	m_scale = scale;
	processWorldMatrix();
}

Vector3D Entity::getPosition()
{
	return m_position;
}

Vector3D Entity::getRotation()
{
	return m_rotation;
}

Vector3D Entity::getScale()
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
