#include "Entity.h"
#include "Game.h"

void Entity::release()
{
	m_game->removeEntity(this);
}

void Entity::processWorldMatrix()
{
	// FIX: createScale/createRotation/createTranslation are STATIC methods that RETURN
	// a new matrix.  The old code called them as instance methods and discarded the
	// return value, so temp stayed identity every time and m_world was always identity.
	// Everything rendered at the origin, getZAxis() always returned (0,0,1), and
	// no movement or rotation was ever visible.
	m_world = Matrix4x4::createScale(m_scale);
	m_world *= Matrix4x4::createRotationX(m_rotation.m_x);
	m_world *= Matrix4x4::createRotationY(m_rotation.m_y);
	m_world *= Matrix4x4::createRotationZ(m_rotation.m_z);
	m_world *= Matrix4x4::createTranslation(m_position);
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
