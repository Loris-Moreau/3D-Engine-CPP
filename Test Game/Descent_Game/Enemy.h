/*
 * Enemy.h
 *
 * Turret-drone enemy.  Each frame it:
 *   1. Rotates to face the player.
 *   2. Fires an EnemyProjectile every kFireInterval seconds when the
 *      player is within kFireRange.
 *
 * Stats:
 *   Health : 75 hp  (3 player laser hits  = 3 × 25 = 75 = dead)
 *   Radius : 18 u   (sphere collision)
 *   Damage : via EnemyProjectile — 34 hp per hit, 3 hits kill player (100 hp)
 */
#pragma once
#include "../All.h"

class Enemy : public MeshEntity
{
public:
    Enemy()  = default;
    ~Enemy() override = default;

    void onCreate() override;
    void onUpdate(float dt) override;

    // Inflict damage; releases (destroys) this entity when health reaches zero.
    void TakeDamage(float dmg);

    float m_health = 75.f;               // 3 player laser hits to kill
    static constexpr float kRadius = 18.f;

private:
    float m_fireTimer = 0.f;             // countdown to next shot

    static constexpr float kFireInterval = 2.f;    // seconds between shots
    static constexpr float kDetectRange  = 1500.f; // wake-up range (units)
    static constexpr float kFireRange    = 1200.f; // shooting range (units)
};
