#include "Missile.h"

#include "Ship.h"

void Missile::onCreate()
{
    auto mesh = createMesh(L"../Assets/Meshes/sphere.obj");
    auto mat = createMaterial(L"../Assets/Shaders/missile.hlsl");

    setMesh(mesh);
    addMaterial(mat);

    setScale(Vector3D(3, 4, 3));
}

void Missile::onUpdate(float deltaTime)
{
    m_elapsed += deltaTime;

    // Move the projectile along the defined direction (ship direction)
    auto pos = m_position + m_dir * deltaTime * 750.0f;
    setPosition(pos);
	
    // After x(3 is good) seconds, delete the projectile
    if (m_elapsed > 3.0f)
    {
        release();
    }
}

void Missile::SetDamage(float InDamage)
{
    m_damage = InDamage;
}

float Missile::GetDamage() const
{
    return m_damage;
}
