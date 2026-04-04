#pragma once

#include "../All.h"

class Ship : public MeshEntity
{
public:
    Ship() = default;
    ~Ship() override = default;

    void onCreate() override;
    void onUpdate(float deltaTime) override;

    void ResetMissileCount();
    void SetMaxMissileCount(unsigned int InMaxCount);
    unsigned int GetMissileCount();

    unsigned int maxMissileCount = 10;
    unsigned int missileCount = 0;

    void SetLaserLevel(unsigned int InLevel);
    unsigned int GetLaserLevel();
    void SetMissileLevel(unsigned int InLevel);
    unsigned int GetMissileLevel();

    // FIX: was 0 — caused damage multiplier to fire every frame at wrong condition
    unsigned int laserLevel   = 1;
    unsigned int missileLevel = 1;

    float GetLaserDamage()   const;
    float GetMissileDamage() const;

    // Health system
    void  TakeDamage(float dmg);
    float GetHealth()    const { return m_health; }
    float GetMaxHealth() const { return m_maxHealth; }

    // Sphere radius used for collision detection and tunnel wall clamping
    static constexpr float kShipRadius = 15.0f;

private:
    float m_pitch = 0.0f;
    float m_yaw   = 0.0f;
    float m_roll  = 0.0f;

    float m_oldPitch = 0.0f;
    float m_oldYaw   = 0.0f;
    float m_oldRoll  = 0.0f;

    float m_camPitch = 0.0f;
    float m_camYaw   = 0.0f;
    float m_camRoll  = 0.0f;

    float m_oldCamPitch = 0.0f;
    float m_oldCamYaw   = 0.0f;
    float m_oldCamRoll  = 0.0f;

    float m_current_cam_distance = 0.0f;
    float m_cam_distance         = 18.0f;

    CameraEntity* m_camera = nullptr;

    // Base damage — level multiplier is computed fresh each frame, never accumulated
    static constexpr float kBaseLaserDamage   = 25.0f;
    static constexpr float kBaseMissileDamage = 50.0f;

    float m_laserDamage   = kBaseLaserDamage;
    float m_missileDamage = kBaseMissileDamage;

    float m_health         = 100.0f;
    float m_maxHealth      = 100.0f;
    float m_invincibleTime = 0.0f; // seconds of invincibility remaining after a hit
};
