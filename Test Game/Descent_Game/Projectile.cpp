#include "Projectile.h"
#include "Ship.h"

void Projectile::onCreate()
{
    auto mesh = createMesh(L"../Assets/Meshes/sphere.obj");
    auto mat  = createMaterial(L"../Assets/Shaders/projectile.hlsl");
    setMesh(mesh);
    addMaterial(mat);
    setScale(Vector3D(2, 2, 2));
    setCollisionRadius(2.0f);
}

void Projectile::onUpdate(float deltaTime)
{
    m_elapsed += deltaTime;

    auto pos = m_position + m_dir * deltaTime * 800.0f;

    // FIX: destroy projectile when it leaves the level geometry (hits a wall).
    // isInBounds() returns false when pos is outside every tunnel zone.
    // Using the projectile's own collision radius so it stops at the wall face.
    if (!getGame()->isInBounds(pos, m_collisionRadius))
    {
        release();
        return;
    }

    setPosition(pos);

    if (m_elapsed > 3.0f)
        release();
}

void Projectile::SetDamage(float d) { m_damage = d; }
float Projectile::GetDamage() const { return m_damage; }
