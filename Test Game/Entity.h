/*
 * Entity.h
 *
 * Base class for everything that exists in the world.
 *
 * World matrix pipeline:
 *   Standard entities (asteroids, wall panels) call setPosition/setRotation/
 *   setScale; each setter rebuilds m_world via processWorldMatrix() using the
 *   static Matrix4x4::create* helpers.
 *
 *   The Ship bypasses this path and writes m_world directly via setWorldMatrix()
 *   because it uses a 4x4 orientation matrix instead of Euler angles.
 *
 * Collision:
 *   Optional sphere radius stored per-entity.  Radius > 0 means the entity
 *   participates in collision detection.  The check itself lives in
 *   DescentGame::checkCollisions().
 */
#pragma once

#include <vector>
#include "Matrix4x4.h"
#include "Rect.h"
#include "Vector3D.h"

class Game;

class Entity
{
public:
    Entity()          = default;
    virtual ~Entity() = default;

    size_t   getId();
    void     getWorldMatrix(Matrix4x4& world);
    void     setPosition(const Vector3D& position);
    void     setRotation(const Vector3D& rotation);
    void     setScale   (const Vector3D& scale);
    Vector3D getPosition();
    Vector3D getRotation();
    Vector3D getScale();
    Game*    getGame();

    MaterialPtr createMaterial(const wchar_t* path);
    TexturePtr  createTexture (const wchar_t* path);
    MeshPtr     createMesh    (const wchar_t* path);

    // ---- Sphere collision radius ----------------------------------------
    // Set > 0 to include this entity in collision checks.
    void  setCollisionRadius(float r)   { m_collisionRadius = r; }
    float getCollisionRadius()  const   { return m_collisionRadius; }
    bool  isCollidable()        const   { return m_collisionRadius > 0.f; }

    // ---- World matrix override ------------------------------------------
    // Writes m_world directly, bypassing the Euler-angle processWorldMatrix
    // path.  Used by Ship to set the camera world matrix from an orientation
    // matrix.  Also updates m_position from the translation row.
    void setWorldMatrix(const Matrix4x4& mat)
    {
        m_world    = mat;
        m_position = { mat.mat[3][0], mat.mat[3][1], mat.mat[3][2] };
    }

    virtual void onCreate()              {}
    virtual void onUpdate(float dt)      {}
    virtual void release();

protected:
    // Rebuilds m_world from m_scale, m_rotation (Euler), and m_position.
    // Called by setPosition/setRotation/setScale.
    void processWorldMatrix();

    Matrix4x4 m_world;
    Vector3D  m_position;
    Vector3D  m_rotation;
    Vector3D  m_scale = { 1, 1, 1 };

    float     m_collisionRadius = 0.f;
    size_t    m_id              = 0;
    Game*     m_game            = nullptr;

    friend class Game;
};
