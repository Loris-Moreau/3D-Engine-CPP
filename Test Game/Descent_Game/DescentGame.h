#pragma once

#include "../All.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Projectile.h"
#include "Missile.h"

#include <vector>
#include <algorithm>
#include <cmath>

// -----------------------------------------------------------------------
// Axis-aligned box defining the interior passable space of a tunnel zone.
// The ship's sphere must stay inside at least one zone.
//
// Zones are stored with their boundaries ALREADY EXTENDED at open faces by
// Ship::kShipRadius so that adjacent zones overlap.  This eliminates the
// "invisible wall" dead-gap that would otherwise appear where two open-faced
// zone boundaries meet at exactly the same plane.
// -----------------------------------------------------------------------
struct TunnelZone
{
    Vector3D min;
    Vector3D max;

    // True if the sphere (centre, r) fits inside this zone on every axis.
    bool containsSphere(const Vector3D& c, float r) const
    {
        return c.m_x >= min.m_x + r && c.m_x <= max.m_x - r
            && c.m_y >= min.m_y + r && c.m_y <= max.m_y - r
            && c.m_z >= min.m_z + r && c.m_z <= max.m_z - r;
    }

    // Nearest legal centre for a sphere of radius r inside this zone.
    Vector3D clampSphere(const Vector3D& c, float r) const
    {
        auto clamp = [](float v, float lo, float hi) {
            return v < lo ? lo : (v > hi ? hi : v);
        };
        return Vector3D(
            clamp(c.m_x, min.m_x + r, max.m_x - r),
            clamp(c.m_y, min.m_y + r, max.m_y - r),
            clamp(c.m_z, min.m_z + r, max.m_z - r)
        );
    }

    // Point-in-zone check (no inset — used for bullets).
    bool containsPoint(const Vector3D& p) const
    {
        return p.m_x >= min.m_x && p.m_x <= max.m_x
            && p.m_y >= min.m_y && p.m_y <= max.m_y
            && p.m_z >= min.m_z && p.m_z <= max.m_z;
    }
};

// -----------------------------------------------------------------------
class DescentGame : public Game
{
public:
    DescentGame()  = default;
    ~DescentGame() override = default;

    void onCreate()         override;
    void onUpdate(float dt) override;
    void onQuit()           override {}

    // Clamp pos so no wall is penetrated (used by Ship).
    Vector3D constrainPosition(const Vector3D& pos, float radius) const override;

    // True when pos is inside the level geometry (used by projectiles).
    bool isInBounds(const Vector3D& pos, float radius) const override;

private:
    bool  m_input = true;
    Ship* m_ship  = nullptr;

    // Passable corridor volumes (extended at open faces — see addZone).
    std::vector<TunnelZone> m_zones;

    // ---- Level building ------------------------------------------------
    void buildMineLevel();

    // Register a passable zone and generate wall panels for its closed faces.
    // openFaces bitmask: bit0=minX  bit1=maxX  bit2=minY(floor)
    //                    bit3=maxY(ceil)  bit4=minZ  bit5=maxZ
    // Open faces get NO wall panel AND the zone boundary is extended by
    // Ship::kShipRadius so adjacent zones always overlap (no dead gap).
    void addZone(const Vector3D& min, const Vector3D& max, int openFaces = 0);

    // Place a single scaled box-mesh wall panel.
    void addWall(const Vector3D& centre, const Vector3D& scale,
                 const wchar_t* texPath = L"../Assets/Textures/wall.jpg");

    // Spawn one asteroid at the given world position with uniform scale.
    Asteroid* spawnAsteroid(const Vector3D& pos, float scale);

    // ---- Collision detection -------------------------------------------
    void checkCollisions();

    static float distSq(const Vector3D& a, const Vector3D& b)
    {
        float dx = a.m_x - b.m_x, dy = a.m_y - b.m_y, dz = a.m_z - b.m_z;
        return dx*dx + dy*dy + dz*dz;
    }
};
