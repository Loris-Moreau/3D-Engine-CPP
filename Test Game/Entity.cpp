/*
 * Entity.cpp
 */
#include "Entity.h"
#include "Game.h"

void Entity::release()
{
    m_game->removeEntity(this);
}

void Entity::processWorldMatrix()
{
    // Build the world matrix as Scale * RotX * RotY * RotZ * Translation.
    // The static Matrix4x4::create* methods RETURN a new matrix each call.
    // They must not be called as instance methods (temp.createScale(...))
    // because that discards the return value — a bug that kept m_world as
    // identity forever, making every entity render at the world origin.
    m_world  = Matrix4x4::createScale(m_scale);
    m_world *= Matrix4x4::createRotationX(m_rotation.m_x);
    m_world *= Matrix4x4::createRotationY(m_rotation.m_y);
    m_world *= Matrix4x4::createRotationZ(m_rotation.m_z);
    m_world *= Matrix4x4::createTranslation(m_position);
}

size_t   Entity::getId()                          { return m_id; }
void     Entity::getWorldMatrix(Matrix4x4& world) { world = m_world; }

void Entity::setPosition(const Vector3D& p) { m_position = p; processWorldMatrix(); }
void Entity::setRotation(const Vector3D& r) { m_rotation = r; processWorldMatrix(); }
void Entity::setScale   (const Vector3D& s) { m_scale    = s; processWorldMatrix(); }

Vector3D Entity::getPosition() { return m_position; }
Vector3D Entity::getRotation() { return m_rotation; }
Vector3D Entity::getScale()    { return m_scale; }
Game*    Entity::getGame()     { return m_game; }

MaterialPtr Entity::createMaterial(const wchar_t* path) { return m_game->createMaterial(path); }
TexturePtr  Entity::createTexture (const wchar_t* path) { return m_game->createTexture(path);  }
MeshPtr     Entity::createMesh    (const wchar_t* path) { return m_game->createMesh(path);     }
