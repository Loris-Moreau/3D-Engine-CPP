/*
 * Enemy.cpp
 *
 * Rotation maths note:
 *   processWorldMatrix() applies Scale * RotX(pitch) * RotY(yaw) * Translation.
 *   For that order the forward vector (getZAxis) equals:
 *       (cos(p)*sin(y),  -sin(p),  cos(p)*cos(y))
 *   Solving for pitch and yaw given a target direction (dx, dy, dz):
 *       pitch = -asinf(dy)
 *       yaw   = atan2f(dx, dz)
 *   This gives exact alignment so getZAxis() == the direction to the player
 *   and can be used directly as the projectile fire direction.
 */
#include "Enemy.h"
#include "EnemyProjectile.h"
#include "Ship.h"
#include <cmath>

void Enemy::onCreate()
{
    // Visual: wall-textured sphere scaled to look like a heavy drone.
    auto tex  = createTexture(L"../Assets/Textures/wall.jpg");
    auto mesh = createMesh(L"../Assets/Meshes/sphere.obj");
    auto mat  = createMaterial(L"../Assets/Shaders/base.hlsl");
    mat->addTexture(tex);
    setMesh(mesh);
    addMaterial(mat);
    setScale(Vector3D(12.f, 12.f, 12.f));
    setCollisionRadius(kRadius);

    // Randomise the initial fire timer so enemies don't all fire simultaneously.
    m_fireTimer = (float)(rand() % 200) / 100.f;  // 0 – 2 s
}

void Enemy::onUpdate(float dt)
{
    // Get a reference to the player ship.  getEntitiesOfType returns an empty
    // vector if the ship has been destroyed (i.e. during a restart frame).
    auto ships = getGame()->getEntitiesOfType<Ship>();
    if (ships.empty()) return;
    Ship* ship = ships[0];

    // --- Direction and distance to player ---
    Vector3D toPlayer = ship->getPosition() - m_position;
    float dx = toPlayer.m_x, dy = toPlayer.m_y, dz = toPlayer.m_z;
    float dist = sqrtf(dx*dx + dy*dy + dz*dz);
    if (dist < 0.001f) return;

    // Normalise
    dx /= dist;  dy /= dist;  dz /= dist;

    // --- Rotate to face the player ---
    // See file-level comment for the derivation of these angles.
    float pitch = -asinf(dy);
    float yaw   = atan2f(dx, dz);
    setRotation(Vector3D(pitch, yaw, 0.f));

    // Only act when the player is close enough to detect.
    if (dist > kDetectRange) return;

    // --- Shoot ---
    m_fireTimer -= dt;
    if (m_fireTimer <= 0.f && dist <= kFireRange)
    {
        m_fireTimer = kFireInterval;

        // Forward axis from the freshly updated world matrix.
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
