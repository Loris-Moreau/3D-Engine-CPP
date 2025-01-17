#pragma once

#include "../All.h"

class Projectile : public MeshEntity
{
public:
    Projectile() = default;
    ~Projectile() override = default;

    void onCreate() override;
    void onUpdate(float deltaTime) override;

    void SetDamage(float InDamage);
    float GetDamage() const;
    
    float m_damage = 0.0f;
    
    Vector3D m_dir;
    float m_elapsed = 0.0f;
};
