#pragma once

#include "../All.h"

class Ship: public MeshEntity
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
    
    unsigned int laserLevel = 0;
    unsigned int missileLevel = 0;

    float GetLaserDamage();
    float GetMissileDamage();
    
private:
    float m_pitch = 0.0f;
    float m_yaw = 0.0f;
    float m_roll = 0.0f;

    float m_oldPitch = 0.0f;
    float m_oldYaw = 0.0f;
    float m_oldRoll = 0.0f;

    float m_camPitch = 0.0f;
    float m_camYaw = 0.0f;
    float m_camRoll = 0.0f;

    float m_oldCamPitch = 0.0f;
    float m_oldCamYaw = 0.0f;
    float m_oldCamRoll = 0.0f;

    
    bool m_turbo_mode = false;
    
    float m_current_cam_distance = 0.0f;
    float m_cam_distance = 18.0f;
    
    CameraEntity* m_camera = nullptr;

    float m_laserDamage = 25.0f;
    float m_missileDamage = 50.0f;
};
