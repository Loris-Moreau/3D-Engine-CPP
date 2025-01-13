#pragma once

#include "../All.h"

class Ship: public MeshEntity
{
public:
    Ship() = default;
    ~Ship() override = default;

    void onCreate() override;
    void onUpdate(float deltaTime) override;

private:
    float m_pitch = 0.0f;
    float m_yaw = 0.0f;

    float m_oldPitch = 0.0f;
    float m_oldYaw = 0.0f;

    float m_camPitch = 0.0f;
    float m_camYaw = 0.0f;

    float m_oldCamPitch = 0.0f;
    float m_oldCamYaw = 0.0f;

    
    bool m_turbo_mode = false;
    
    float m_current_cam_distance = 0.0f;
    float m_cam_distance = 18.0f;
    
    CameraEntity* m_camera = nullptr;
};
