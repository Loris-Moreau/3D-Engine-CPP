#pragma once

#include "../All.h"

class Missile : public MeshEntity
{
public:
    Missile() = default;
    ~Missile() override = default;

    void onCreate() override;
    void onUpdate(float deltaTime) override;
    
    void SetDamage(float InDamage);
    float GetDamage() const;

    float m_damage = 0.0f;
    
    Vector3D m_dir;
    float m_elapsed = 0.0f;
};
