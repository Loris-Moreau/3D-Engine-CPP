#pragma once

/*
 * DescentGame.h
 *
 * Level layout (top-down, +Z south, +X east):
 *
 *          [N_ROOM]
 *             |
 *          [N_ARM]
 *             |
 *  [W_ROOM]-[W_ARM]-[CENTER]-[E_ARM]-[E_ROOM]
 *                      |
 *                   [S_ARM]
 *                      |
 *                   [S_ROOM]  <- player spawns here (Z = 2300)
 *
 *  East arm end: upward shaft   → [UPPER_E_ROOM]
 *  West arm end: downward shaft → [LOWER_W_ROOM]
 *
 *  Wall seam fix (addSegWalls):
 *    Panels extend by T only at CLOSED faces.  Open-face ends stop flush
 *    with the zone boundary so adjacent panels never overlap → no Z-fighting.
 *    The east and west shaft sections are generated panel-by-panel (not via
 *    addSegWalls) so their west/east open faces can be handled individually.
 */

#include "../All.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Enemy.h"
#include "EnemyProjectile.h"
#include "Projectile.h"
#include "Missile.h"
#include <vector>
#include <algorithm>

// ---------------------------------------------------------------------------
// TunnelZone — one passable box-shaped region of the mine.
// Stored boundaries are extended at open faces by Ship::kShipRadius so
// adjacent zones always overlap, preventing invisible-wall dead-gaps.
// ---------------------------------------------------------------------------
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

    Vector3D constrainPosition(const Vector3D& pos, float r) const override;
    bool     isInBounds       (const Vector3D& pos, float r) const override;

private:
    bool  m_input          = true;
    Ship* m_ship           = nullptr;

    // Player-death restart: when the ship's health hits zero we wait
    // kRestartDelay seconds then call restartLevel().
    bool  m_restartPending = false;
    float m_restartTimer   = 0.f;
    static constexpr float kRestartDelay = 1.5f;

    std::vector<TunnelZone> m_zones;

    void buildMineLevel();
    void restartLevel();   // clears all entities and calls onCreate()

    // openFaces bits: 0=minX  1=maxX  2=floor(minY)  3=ceil(maxY)  4=minZ  5=maxZ
    void addZone    (const Vector3D& mn, const Vector3D& mx, int openFaces = 0);
    void addSegWalls(const Vector3D& mn, const Vector3D& mx,
                     int openFaces, float T,
                     const wchar_t* wallTex, const wchar_t* floorTex);
    void addPanel   (const Vector3D& centre, const Vector3D& scale,
                     const wchar_t* texPath = L"../Assets/Textures/wall.jpg");

    Asteroid* spawnAsteroid(const Vector3D& pos, float scale);
    Enemy*    spawnEnemy   (const Vector3D& pos);

    void checkCollisions();

    static float distSq(const Vector3D& a, const Vector3D& b)
    {
        float dx=a.m_x-b.m_x, dy=a.m_y-b.m_y, dz=a.m_z-b.m_z;
        return dx*dx+dy*dy+dz*dz;
    }
};
