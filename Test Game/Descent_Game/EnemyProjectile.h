/*
 * EnemyProjectile.h
 *
 * Bolt fired by Enemy entities.
 *   Speed   : 500 u/s  (slower than player laser — gives time to dodge)
 *   Damage  : 34 hp    (3 hits kill the player: 3 × 34 = 102 > 100)
 *   Lifetime: 4 s or until wall impact
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

    float GetDamage() const { return kDamage; }

    Vector3D m_dir;           // unit vector, set by Enemy at spawn

private:
    float m_elapsed = 0.f;
    static constexpr float kDamage   = 34.f;
    static constexpr float kSpeed    = 500.f;
    static constexpr float kLifetime = 4.f;
};
