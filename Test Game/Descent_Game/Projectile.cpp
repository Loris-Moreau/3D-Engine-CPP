/*
 * Projectile.cpp
 */
#include "Projectile.h"
#include "Ship.h"

void Projectile::onCreate()
{
    auto mesh = createMesh(L"../Assets/Meshes/sphere.obj");
    auto mat  = createMaterial(L"../Assets/Shaders/projectile.hlsl");
    setMesh(mesh);
    addMaterial(mat);
    setScale(Vector3D(2, 2, 2));
    setCollisionRadius(2.f);
}

void Projectile::onUpdate(float dt)
{
    m_elapsed += dt;

    const Vector3D pos = m_position + m_dir * dt * 800.f;

    // Destroy the projectile the moment it would move outside the level.
    // isInBounds() checks the projectile's collision sphere against all
    // tunnel zones; returning false means a wall has been hit.
    if (!getGame()->isInBounds(pos, m_collisionRadius))
    {
        release();
        return;
    }

    setPosition(pos);

    // Lifetime cap — destroy after 3 s even if nothing was hit.
    if (m_elapsed > 3.f) release();
}

void  Projectile::SetDamage(float d) { m_damage = d; }
float Projectile::GetDamage() const  { return m_damage; }
