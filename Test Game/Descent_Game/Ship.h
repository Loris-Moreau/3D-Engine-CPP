/*
 * Ship.h
 *
 * Player-controlled spaceship entity.
 *
 * Orientation model:
 *   Orientation is stored as a 4x4 rotation matrix (m_orientMat) rather
 *   than Euler angles.  Rotation inputs PRE-multiply this matrix, which in
 *   the engine's row-vector convention (v' = v*M) equals a LOCAL-SPACE
 *   rotation around the ship's own axes.  This guarantees that left/right
 *   mouse always turns the ship left/right regardless of how much roll has
 *   been accumulated — the classic Descent 6-DOF feel.
 *
 * Weapon balance:
 *   Laser  (left-click,  Projectile) : 25 damage — 3 hits to kill an asteroid
 *   Missile(right-click, Missile)    : 75 damage — 1 hit  to kill an asteroid
 *   Asteroid health                  : 75 hp
 */

#pragma once
#include "../All.h"

class Ship : public MeshEntity
{
public:
    Ship()  = default;
    ~Ship() override = default;

    void onCreate()         override;
    void onUpdate(float dt) override;

    // ---- Missile inventory -----------------------------------------------
    void         ResetMissileCount();
    void         SetMaxMissileCount(unsigned int n);
    unsigned int GetMissileCount();

    unsigned int maxMissileCount = 10;   // maximum missiles the ship can carry
    unsigned int missileCount    = 0;    // missiles currently in flight

    // ---- Weapon levels (1 = base, higher = scaled damage) ----------------
    void         SetLaserLevel  (unsigned int n);
    void         SetMissileLevel(unsigned int n);
    unsigned int GetLaserLevel  () { return m_laserLevel;   }
    unsigned int GetMissileLevel() { return m_missileLevel; }

    float GetLaserDamage  () const { return m_laserDamage;   }
    float GetMissileDamage() const { return m_missileDamage; }

    // ---- Health ----------------------------------------------------------
    void  TakeDamage(float dmg);
    float GetHealth   () const { return m_health;    }
    float GetMaxHealth() const { return m_maxHealth; }
    bool  IsDead      () const { return m_health <= 0.f; }

    // Sets the ship's facing direction at spawn without fighting the smooth
    // rotation accumulator.  Call once after createEntity<Ship>().
    void setInitialYaw(float yaw);

    // Sphere radius used for wall clamping and asteroid ramming detection.
    static constexpr float kShipRadius = 15.f;

private:
    // ---- Orientation (6-DOF matrix) -------------------------------------
    // m_orientMat  : pure rotation, rows = right/up/forward axes.
    // m_camOrientMat: same but exponentially lagged behind the ship for
    //                 a smooth camera follow effect.
    // m_reorthTimer: seconds since the last Gram-Schmidt pass; applied every
    //                0.5 s to counter floating-point drift accumulation.
    Matrix4x4 m_orientMat;
    Matrix4x4 m_camOrientMat;
    float     m_reorthTimer = 0.f;

    // ---- Camera ---------------------------------------------------------
    float         m_current_cam_distance = 18.f;   // interpolated towards m_cam_distance
    float         m_cam_distance         = 18.f;   // target distance (changes with speed)
    CameraEntity* m_camera               = nullptr;

    // ---- Weapons --------------------------------------------------------
    unsigned int m_laserLevel   = 1;
    unsigned int m_missileLevel = 1;

    // Base damage values.  Actual damage = base * (1 + (level-1) * 0.1).
    static constexpr float kBaseLaser   = 25.f;
    static constexpr float kBaseMissile = 75.f;  // one-shots an asteroid (health = 75)
    float m_laserDamage   = kBaseLaser;
    float m_missileDamage = kBaseMissile;

    // ---- Health ---------------------------------------------------------
    float m_health         = 100.f;
    float m_maxHealth      = 100.f;
    float m_invincibleTime = 0.f;   // seconds of post-hit invincibility remaining
};
