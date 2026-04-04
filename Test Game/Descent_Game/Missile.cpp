#include "Missile.h"

#include "Ship.h"

void Missile::onCreate()
{
    auto mesh = createMesh(L"../Assets/Meshes/sphere.obj");
    auto mat  = createMaterial(L"../Assets/Shaders/missile.hlsl");

    setMesh(mesh);
    addMaterial(mat);
    setScale(Vector3D(3, 4, 3));

    // Slightly larger collision sphere than projectile
    setCollisionRadius(4.0f);
}

void Missile::onUpdate(float deltaTime)
{
    m_elapsed += deltaTime;

    auto pos = m_position + m_dir * deltaTime * 750.0f;
    setPosition(pos);

    if (m_elapsed > 3.0f)
        release();
}

void Missile::SetDamage(float d) { m_damage = d; }
float Missile::GetDamage() const { return m_damage; }
