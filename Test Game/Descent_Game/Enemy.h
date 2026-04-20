/*
 * Enemy.h  — Turret-drone enemy
 *
 * Every frame: rotates to face the player, fires an EnemyProjectile every
 * kFireInterval seconds when the player is within kFireRange.
 *
 * Damage model:
 *   Health  : 75 hp  (3 × player laser 25hp = dead)
 *   Fires   : EnemyProjectile  34 hp each  (3 hits kills player at 100 hp)
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

    // Reduce health; releases the entity when it reaches zero.
    void TakeDamage(float dmg);

    float m_health = 75.f;
    static constexpr float kRadius = 18.f;

private:
    float m_fireTimer = 0.f;

    static constexpr float kFireInterval = 2.f;    // seconds between shots
    static constexpr float kDetectRange  = 1600.f; // wake-up range
    static constexpr float kFireRange    = 1200.f; // maximum shooting range
};
