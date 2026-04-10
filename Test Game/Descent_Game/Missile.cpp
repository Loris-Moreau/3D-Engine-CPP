/*
 * Missile.cpp
 */
#include "Missile.h"
#include "Ship.h"

void Missile::onCreate()
{
    auto mesh = createMesh(L"../Assets/Meshes/sphere.obj");
    auto mat  = createMaterial(L"../Assets/Shaders/missile.hlsl");
    setMesh(mesh);
    addMaterial(mat);
    setScale(Vector3D(3, 4, 3));
    setCollisionRadius(4.f);
}

void Missile::onUpdate(float dt)
{
    m_elapsed += dt;

    const Vector3D pos = m_position + m_dir * dt * 750.f;

    // Same wall-impact logic as Projectile — destroy on leaving the level.
    if (!getGame()->isInBounds(pos, m_collisionRadius))
    {
        release();
        return;
    }

    setPosition(pos);

    if (m_elapsed > 3.f) release();
}

void  Missile::SetDamage(float d) { m_damage = d; }
float Missile::GetDamage() const  { return m_damage; }
