/*
 * Missile.h
 *
 * Heavy missile fired by right-click.
 *   Speed  : 750 u/s
 *   Damage : 75 hp — one-shots an asteroid (health = 75)
 *   Lifetime: 3 s or until it exits the level geometry
 *   Max in flight: controlled by Ship::maxMissileCount
 */
#pragma once
#include "../All.h"

class Missile : public MeshEntity
{
public:
    Missile() = default;
    ~Missile() override = default;

    void onCreate() override;
    void onUpdate(float deltaTime) override;

    void  SetDamage(float d);
    float GetDamage() const;

    Vector3D m_dir;            // unit vector set by Ship at spawn time
    float    m_damage  = 0.f;
    float    m_elapsed = 0.f;  // seconds since spawn
};
