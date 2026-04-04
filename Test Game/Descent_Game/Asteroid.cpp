#include "Asteroid.h"

#include <cstdlib>
#include <cmath>

void Asteroid::onCreate()
{
    auto tex  = createTexture(L"../Assets/Textures/asteroid.jpg");
    auto mesh = createMesh(L"../Assets/Meshes/asteroid.obj");
    auto mat  = createMaterial(L"../Assets/Shaders/base.hlsl");
    mat->addTexture(tex);

    setMesh(mesh);
    addMaterial(mat);

    // Random tumble axis (normalised)
    float ax = (float)(rand() % 200 - 100);
    float ay = (float)(rand() % 200 - 100);
    float az = (float)(rand() % 200 - 100);
    float len = sqrtf(ax*ax + ay*ay + az*az);
    if (len < 0.001f) len = 1.0f;
    m_tumbleAxis  = Vector3D(ax/len, ay/len, az/len);
    m_tumbleSpeed = 0.15f + (float)(rand() % 60) / 100.0f; // 0.15 – 0.75 rad/s

    // Collision radius reflects the entity's scale (set by caller before/after onCreate)
    // Updated via refreshCollisionRadius() — but we set a sensible default here.
    // Callers should call setCollisionRadius after setScale.
    setCollisionRadius(getScale().m_x * 5.5f);
}

void Asteroid::onUpdate(float deltaTime)
{
    // Lazy tumble — just rotate around the random axis via Euler approximation
    m_rotation.m_x += m_tumbleAxis.m_x * m_tumbleSpeed * deltaTime;
    m_rotation.m_y += m_tumbleAxis.m_y * m_tumbleSpeed * deltaTime;
    m_rotation.m_z += m_tumbleAxis.m_z * m_tumbleSpeed * deltaTime;
    setRotation(m_rotation);
}

void Asteroid::TakeDamage(float dmg)
{
    m_health -= dmg;
    if (m_health <= 0.0f)
        release();
}
