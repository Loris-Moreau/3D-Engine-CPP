#pragma once

#include "../All.h"

// Asteroid — a destructible obstacle that floats through the mine.
// Collision is sphere-based; radius is set in onCreate() from the scale.
class Asteroid : public MeshEntity
{
public:
    Asteroid() = default;
    ~Asteroid() override = default;

    void onCreate() override;
    void onUpdate(float deltaTime) override;

    // Inflict damage; destroys the asteroid when health reaches zero.
    void TakeDamage(float dmg);

    float m_health = 80.0f;

private:
    // Slow tumble axis and speed, randomised on creation
    Vector3D m_tumbleAxis;
    float    m_tumbleSpeed = 0.3f;
};
