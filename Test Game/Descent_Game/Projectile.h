#pragma once

#include "../All.h"

class Projectile : public MeshEntity
{
public:
    Projectile() = default;
    ~Projectile() override = default;

    void onCreate() override;
    void onUpdate(float deltaTime) override;

    Vector3D m_dir;
    float m_elapsed = 0.0f;
};
