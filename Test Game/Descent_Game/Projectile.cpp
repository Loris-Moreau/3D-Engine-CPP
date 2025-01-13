#include "Projectile.h"
#include "Ship.h"

void Projectile::onCreate()
{
    auto mesh = createMesh(L"Assets/Meshes/sphere.obj");
    auto mat = createMaterial(L"Assets/Shaders/projectile.hlsl");

    setMesh(mesh);
    addMaterial(mat);

    setScale(Vector3D(2, 2, 2));
}

void Projectile::onUpdate(float deltaTime)
{
    m_elapsed += deltaTime;

    // Move the projectile along the defined direction (ship direction)
    auto pos = m_position + m_dir * deltaTime * 800.0f;
    setPosition(pos);
	
    // After x(3 is good) seconds, delete the projectile
    if (m_elapsed > 3.0f)
    {
        release();
    }
}
