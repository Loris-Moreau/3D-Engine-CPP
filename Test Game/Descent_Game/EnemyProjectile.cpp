/*
 * EnemyProjectile.cpp
 */
#include "EnemyProjectile.h"

void EnemyProjectile::onCreate()
{
    // Use missile.hlsl for a visually distinct colour from the player laser.
    auto mesh = createMesh(L"../Assets/Meshes/sphere.obj");
    auto mat  = createMaterial(L"../Assets/Shaders/missile.hlsl");
    setMesh(mesh);
    addMaterial(mat);
    setScale(Vector3D(3.f, 3.f, 3.f));
    setCollisionRadius(3.f);
}

void EnemyProjectile::onUpdate(float dt)
{
    m_elapsed += dt;

    Vector3D pos = m_position + m_dir * (kSpeed * dt);

    // Destroy on wall impact — same mechanism as player projectiles.
    if (!getGame()->isInBounds(pos, m_collisionRadius))
    {
        release();
        return;
    }

    setPosition(pos);
    if (m_elapsed > kLifetime) release();
}
