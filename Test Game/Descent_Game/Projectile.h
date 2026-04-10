/*
 * Projectile.h
 *
 * Fast laser bolt fired by left-click.
 *   Speed  : 800 u/s
 *   Damage : 25 hp (set by Ship via SetDamage)
 *   Lifetime: 3 s or until it exits the level geometry
 */
#pragma once
#include "../All.h"

class Projectile : public MeshEntity
{
public:
    Projectile() = default;
    ~Projectile() override = default;

    void onCreate() override;
    void onUpdate(float deltaTime) override;

    void  SetDamage(float d);
    float GetDamage() const;

    Vector3D m_dir;            // unit vector set by Ship at spawn time
    float    m_damage  = 0.f;
    float    m_elapsed = 0.f;  // seconds since spawn
};
