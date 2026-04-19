#pragma once
#include "../All.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Enemy.h"
#include "EnemyProjectile.h"
#include "Projectile.h"
#include "Missile.h"
#include <vector>
#include <algorithm>

/*
 * TunnelZone — axis-aligned box defining one passable region of the mine.
 *
 * Stored boundaries are extended at open faces by Ship::kShipRadius so
 * adjacent zones always overlap — eliminating dead-gaps at transitions.
 * containsSphere insets by r so the ship's edge, not its centre, is tested.
 */
struct TunnelZone
{
    Vector3D min, max;

    bool containsSphere(const Vector3D& c, float r) const
    {
        return c.m_x >= min.m_x+r && c.m_x <= max.m_x-r
            && c.m_y >= min.m_y+r && c.m_y <= max.m_y-r
            && c.m_z >= min.m_z+r && c.m_z <= max.m_z-r;
    }

    Vector3D clampSphere(const Vector3D& c, float r) const
    {
        auto cl = [](float v, float lo, float hi){ return v<lo?lo:(v>hi?hi:v); };
        return { cl(c.m_x,min.m_x+r,max.m_x-r),
                 cl(c.m_y,min.m_y+r,max.m_y-r),
                 cl(c.m_z,min.m_z+r,max.m_z-r) };
    }
};

class DescentGame : public Game
{
public:
    DescentGame()  = default;
    ~DescentGame() override = default;

    void onCreate()         override;
    void onUpdate(float dt) override;
    void onQuit()           override {}

    // Game overrides
    Vector3D constrainPosition(const Vector3D& pos, float r) const override;
    bool     isInBounds       (const Vector3D& pos, float r) const override;

private:
    bool  m_input          = true;
    Ship* m_ship           = nullptr;

    // Level restart — triggered when the player's health reaches zero.
    bool  m_restartPending = false;
    float m_restartTimer   = 0.f;   // delay before restart (gives a death pause)

    std::vector<TunnelZone> m_zones;

    void buildMineLevel();
    void restartLevel();

    // --- Level-building helpers ---
    // openFaces bits: 0=minX  1=maxX  2=floor(minY)  3=ceil(maxY)  4=minZ  5=maxZ
    void addZone    (const Vector3D& mn, const Vector3D& mx, int openFaces = 0);
    void addSegWalls(const Vector3D& mn, const Vector3D& mx,
                     int openFaces, float T,
                     const wchar_t* wallTex, const wchar_t* floorTex);
    void addPanel   (const Vector3D& centre, const Vector3D& scale,
                     const wchar_t* texPath = L"../Assets/Textures/wall.jpg");

    Asteroid* spawnAsteroid(const Vector3D& pos, float scale);
    Enemy*    spawnEnemy   (const Vector3D& pos);

    // --- Per-frame collision detection ---
    void checkCollisions();

    static float distSq(const Vector3D& a, const Vector3D& b)
    {
        float dx=a.m_x-b.m_x, dy=a.m_y-b.m_y, dz=a.m_z-b.m_z;
        return dx*dx+dy*dy+dz*dz;
    }
};
