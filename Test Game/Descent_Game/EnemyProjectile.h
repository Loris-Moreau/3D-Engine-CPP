/*
 * EnemyProjectile.h
 *
 * Projectile fired by Enemy entities.
 *
 * Stats:
 *   Speed  : 500 u/s  (slower than player laser — gives time to dodge)
 *   Damage : 34 hp    (3 hits = 102 > 100 hp = player dead on 3rd hit)
 *   Lifetime: 4 s     (or until it exits the level geometry)
 */
#pragma once
#include "../All.h"

class EnemyProjectile : public MeshEntity
{
public:
    EnemyProjectile()  = default;
    ~EnemyProjectile() override = default;

    void onCreate() override;
    void onUpdate(float dt) override;

    float    GetDamage() const { return kDamage; }
    float    getCollisionRadius() const { return m_collisionRadius; }

    Vector3D m_dir;              // unit vector, set by Enemy at spawn

private:
    float m_elapsed = 0.f;
    static constexpr float kDamage   = 34.f;   // 3 hits kill the player
    static constexpr float kSpeed    = 500.f;
    static constexpr float kLifetime = 4.f;
};
