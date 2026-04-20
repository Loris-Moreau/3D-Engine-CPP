/*
 * Enemy.cpp
 *
 * Rotation maths:
 *   processWorldMatrix builds  Scale * RotX(pitch) * RotY(yaw) * T.
 *   For that order the world-space forward vector (getZAxis) equals:
 *       (cos(p)*sin(y),  -sin(p),  cos(p)*cos(y))
 *   Inverting: given a unit direction (dx, dy, dz):
 *       pitch = -asinf(dy)
 *       yaw   = atan2f(dx, dz)
 *   So setRotation(pitch, yaw, 0) makes getZAxis() == direction to player.
 */
#include "Enemy.h"
#include "EnemyProjectile.h"
#include "Ship.h"
#include <cmath>

void Enemy::onCreate()
{
    auto tex  = createTexture(L"../Assets/Textures/wall.jpg");
    auto mesh = createMesh(L"../Assets/Meshes/sphere.obj");
    auto mat  = createMaterial(L"../Assets/Shaders/base.hlsl");
    mat->addTexture(tex);
    setMesh(mesh);
    addMaterial(mat);
    setScale(Vector3D(12.f, 12.f, 12.f));
    setCollisionRadius(kRadius);

    // Stagger fire timers so enemies don't all shoot simultaneously.
    m_fireTimer = (float)(rand() % 200) / 100.f;
}

void Enemy::onUpdate(float dt)
{
    auto ships = getGame()->getEntitiesOfType<Ship>();
    if (ships.empty()) return;
    Ship* ship = ships[0];

    // Direction to player
    Vector3D toPlayer = ship->getPosition() - m_position;
    float dx = toPlayer.m_x, dy = toPlayer.m_y, dz = toPlayer.m_z;
    float dist = sqrtf(dx*dx + dy*dy + dz*dz);
    if (dist < 0.001f) return;
    dx /= dist;  dy /= dist;  dz /= dist;

    // Rotate to face the player (see file-level maths note)
    float pitch = -asinf(dy);
    float yaw   = atan2f(dx, dz);
    setRotation(Vector3D(pitch, yaw, 0.f));

    if (dist > kDetectRange) return;

    // Fire a bolt when in range and timer expires
    m_fireTimer -= dt;
    if (m_fireTimer <= 0.f && dist <= kFireRange)
    {
        m_fireTimer = kFireInterval;

        // Forward axis from the freshly computed world matrix
        Vector3D fireDir = m_world.getZAxis();

        auto bolt = getGame()->createEntity<EnemyProjectile>();
        if (bolt)
        {
            bolt->m_dir = fireDir;
            bolt->setPosition(m_position);
        }
    }
}

void Enemy::TakeDamage(float dmg)
{
    m_health -= dmg;
    if (m_health <= 0.f) release();
}
