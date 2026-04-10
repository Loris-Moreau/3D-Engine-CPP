/*
 * Asteroid.cpp
 */
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

    // Generate a random normalised tumble axis.
    float ax = (float)(rand() % 200 - 100);
    float ay = (float)(rand() % 200 - 100);
    float az = (float)(rand() % 200 - 100);
    float len = sqrtf(ax*ax + ay*ay + az*az);
    if (len < 0.001f) len = 1.f;
    m_tumbleAxis  = { ax/len, ay/len, az/len };
    m_tumbleSpeed = 0.15f + (float)(rand() % 60) / 100.f;   // 0.15 – 0.75 rad/s

    // Default collision radius from the current scale.
    // spawnAsteroid() calls setCollisionRadius() again after setScale(),
    // so this default is only used if the caller forgets to set scale first.
    setCollisionRadius(getScale().m_x * 5.5f);
}

void Asteroid::onUpdate(float dt)
{
    // Euler-angle tumble around the random axis.  Simple and cheap for a rock.
    m_rotation.m_x += m_tumbleAxis.m_x * m_tumbleSpeed * dt;
    m_rotation.m_y += m_tumbleAxis.m_y * m_tumbleSpeed * dt;
    m_rotation.m_z += m_tumbleAxis.m_z * m_tumbleSpeed * dt;
    setRotation(m_rotation);
}

void Asteroid::TakeDamage(float dmg)
{
    m_health -= dmg;
    if (m_health <= 0.f) release();
}
