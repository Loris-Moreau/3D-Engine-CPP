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
// -----------------------------------------------------------------------
struct TunnelZone
{
    Vector3D min;
    Vector3D max;

    bool containsSphere(const Vector3D& centre, float r) const
    {
        return centre.m_x >= min.m_x + r && centre.m_x <= max.m_x - r
            && centre.m_y >= min.m_y + r && centre.m_y <= max.m_y - r
            && centre.m_z >= min.m_z + r && centre.m_z <= max.m_z - r;
    }

    // Nearest point on the (inset) box to centre
    Vector3D clampSphere(const Vector3D& centre, float r) const
    {
        auto clamp = [](float v, float lo, float hi) {
            return v < lo ? lo : (v > hi ? hi : v);
        };
        return Vector3D(
            clamp(centre.m_x, min.m_x + r, max.m_x - r),
            clamp(centre.m_y, min.m_y + r, max.m_y - r),
            clamp(centre.m_z, min.m_z + r, max.m_z - r)
        );
    }
};

// -----------------------------------------------------------------------
class DescentGame : public Game
{
public:
    DescentGame()  = default;
    ~DescentGame() override = default;

    void onCreate()            override;
    void onUpdate(float dt)    override;
    void onQuit()              override {}

    // Game::constrainPosition override — projects pos inside the tunnel zone union.
    Vector3D constrainPosition(const Vector3D& pos, float radius) const override;

private:
    bool  m_input = true;
    Ship* m_ship  = nullptr;

    std::vector<TunnelZone> m_zones;        // passable corridor volumes
    std::vector<Asteroid*>  m_asteroids;    // tracked so we can do collision checks

    // ---- Level building helpers ----------------------------------------
    void buildMineLevel();

    // Adds a TunnelZone and also constructs the 6 wall panels for it.
    // openFaces bitmask: bit0=minX, bit1=maxX, bit2=minY, bit3=maxY, bit4=minZ, bit5=maxZ
    // Set a bit to leave that face open (tunnel entrance/exit).
    void addZone(const Vector3D& min, const Vector3D& max, int openFaces = 0);

    // Spawns a wall panel (scaled box mesh) at the given world position/scale.
    void addWall(const Vector3D& centre, const Vector3D& scale,
                 const wchar_t* texPath = L"../Assets/Textures/wall.jpg");

    // Spawns an asteroid at a given position/scale
    Asteroid* spawnAsteroid(const Vector3D& pos, float scale);

    // ---- Collision detection -------------------------------------------
    void checkCollisions();
    static float distSq(const Vector3D& a, const Vector3D& b)
    {
        float dx = a.m_x - b.m_x, dy = a.m_y - b.m_y, dz = a.m_z - b.m_z;
        return dx*dx + dy*dy + dz*dz;
    }
};
