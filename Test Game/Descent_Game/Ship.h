#pragma once
#include "../All.h"

class Ship : public MeshEntity
{
public:
    Ship()  = default;
    ~Ship() override = default;

    void onCreate()              override;
    void onUpdate(float dt)      override;

    void ResetMissileCount();
    void SetMaxMissileCount(unsigned int n);
    unsigned int GetMissileCount();

    unsigned int maxMissileCount = 10;
    unsigned int missileCount    = 0;

    void SetLaserLevel  (unsigned int n);
    void SetMissileLevel(unsigned int n);
    unsigned int GetLaserLevel  () { return m_laserLevel;   }
    unsigned int GetMissileLevel() { return m_missileLevel; }

    float GetLaserDamage  () const { return m_laserDamage;   }
    float GetMissileDamage() const { return m_missileDamage; }

    void  TakeDamage(float dmg);
    float GetHealth   () const { return m_health;    }
    float GetMaxHealth() const { return m_maxHealth; }
    bool  IsDead      () const { return m_health <= 0.f; }

    // Sets initial facing without fighting the rotation accumulator.
    void setInitialYaw(float yaw);

    static constexpr float kShipRadius = 15.f;

private:
    // -----------------------------------------------------------------------
    //  6-DOF orientation stored as a pure rotation 4x4 matrix.
    //  Rotation inputs PRE-multiply this matrix which, in the engine's
    //  row-vector convention (v' = v*M), is equivalent to a LOCAL-SPACE
    //  rotation around the ship's own axes.  This ensures mouse left/right
    //  always turns the ship left/right regardless of roll.
    // -----------------------------------------------------------------------
    Matrix4x4 m_orientMat;       // ship orientation (identity = facing +Z)
    Matrix4x4 m_camOrientMat;    // camera orientation (lags behind ship)
    float     m_reorthTimer = 0.f;

    float m_current_cam_distance = 18.f;
    float m_cam_distance         = 18.f;

    CameraEntity* m_camera = nullptr;

    unsigned int m_laserLevel   = 1;
    unsigned int m_missileLevel = 1;

    static constexpr float kBaseLaser   = 25.f;
    static constexpr float kBaseMissile = 50.f;
    float m_laserDamage   = kBaseLaser;
    float m_missileDamage = kBaseMissile;

    float m_health         = 100.f;
    float m_maxHealth      = 100.f;
    float m_invincibleTime = 0.f;
};
