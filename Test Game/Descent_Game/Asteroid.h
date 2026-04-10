/*
 * Asteroid.h
 *
 * Destructible rock obstacle.  Health = 75:
 *   - Laser  (Projectile, 25 dmg) : 3 hits to destroy
 *   - Missile(Missile,   75 dmg) : 1 hit  to destroy
 *
 * Collision is sphere-based.  The radius is set by the spawner after
 * setScale(), using the formula: radius = scale * 5.5.
 */
#pragma once
#include "../All.h"

class Asteroid : public MeshEntity
{
public:
    Asteroid() = default;
    ~Asteroid() override = default;

    void onCreate() override;
    void onUpdate(float deltaTime) override;

    // Reduces health by dmg.  Calls release() (destroys entity) at zero.
    void TakeDamage(float dmg);

    // Health is public so DescentGame::checkCollisions can read it without
    // a getter (avoids an extra function call in the hot collision loop).
    float m_health = 75.f;   // 3 laser hits or 1 missile to destroy

private:
    Vector3D m_tumbleAxis;            // random normalised axis, set in onCreate
    float    m_tumbleSpeed = 0.3f;   // radians per second, randomised in onCreate
};
