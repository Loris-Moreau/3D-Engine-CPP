#pragma once

/*
 * DescentGame.h
 *
 * Concrete Game subclass that implements the Descent mine level.
 * Responsibilities:
 *   - Level geometry: builds wall panels and asteroid clusters on startup.
 *   - Collision zones: axis-aligned boxes (TunnelZone) that define the
 *     passable space; used every frame by constrainPosition() and isInBounds().
 *   - Per-frame collision detection: projectile/missile vs asteroid,
 *     ship vs asteroid (ramming).
 *
 * Level layout (top-down, +Z south, +X east):
 *
 *          [N_ROOM]
 *             |
 *          [N_ARM]           Z: -2000 to -200
 *             |
 *  [W_ROOM]-[W_ARM]-[CENTER]-[E_ARM]-[E_ROOM]
 *                      |
 *                   [S_ARM]  Z: +200 to +2000
 *                      |
 *                   [S_ROOM] <- player spawns here (Z = 2300)
 *
 *  Vertical element east: east arm end opens upward into [UPPER_E_ROOM].
 *  Vertical element west: west arm end opens downward into [LOWER_W_ROOM].
 *
 *  All corridors and rooms share the same cross-section (400 u wide x 260 u
 *  tall), so wall panels always connect flush at junctions.
 */

#include "../All.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Projectile.h"
#include "Missile.h"
#include <vector>
#include <algorithm>

// ---------------------------------------------------------------------------
// TunnelZone
//
// Axis-aligned box representing one passable region of the mine.
// Stored boundaries are SLIGHTLY LARGER than the visual geometry at open
// faces (extended by Ship::kShipRadius) so adjacent zones always overlap —
// eliminating any dead-gap at zone transitions.
// ---------------------------------------------------------------------------
struct TunnelZone
{
    Vector3D min, max;

    // True when sphere (centre c, radius r) fits entirely inside this zone.
    // Insets each axis by r so the ship's edge, not its centre, is checked.
    bool containsSphere(const Vector3D& c, float r) const
    {
        return c.m_x >= min.m_x + r && c.m_x <= max.m_x - r
            && c.m_y >= min.m_y + r && c.m_y <= max.m_y - r
            && c.m_z >= min.m_z + r && c.m_z <= max.m_z - r;
    }

    // Returns the nearest position inside this zone for a sphere of radius r.
    // Used to slide the ship along a wall instead of stopping it dead.
    Vector3D clampSphere(const Vector3D& c, float r) const
    {
        auto cl = [](float v, float lo, float hi){ return v < lo ? lo : (v > hi ? hi : v); };
        return { cl(c.m_x, min.m_x + r, max.m_x - r),
                 cl(c.m_y, min.m_y + r, max.m_y - r),
                 cl(c.m_z, min.m_z + r, max.m_z - r) };
    }
};

// ---------------------------------------------------------------------------
class DescentGame : public Game
{
public:
    DescentGame()  = default;
    ~DescentGame() override = default;

    void onCreate()         override;   // spawns level geometry + ship
    void onUpdate(float dt) override;   // runs collision detection each frame
    void onQuit()           override {}

    // Game overrides — level-geometry versions of the virtual wall interface.
    Vector3D constrainPosition(const Vector3D& pos, float r) const override;
    bool     isInBounds       (const Vector3D& pos, float r) const override;

private:
    bool  m_input = true;     // true = play mode (cursor locked, input active)
    Ship* m_ship  = nullptr;  // raw pointer to the player ship entity

    // All passable volumes in the mine.  Built once in buildMineLevel(),
    // iterated every frame by constrainPosition() and isInBounds().
    std::vector<TunnelZone> m_zones;

    // ---- Level building -------------------------------------------------

    void buildMineLevel();

    // Registers a passable collision zone.
    // Each bit in openFaces marks one face that connects to a neighbouring
    // zone; that face gets no wall panel and the stored zone boundary is
    // extended by Ship::kShipRadius so adjacent zones always overlap.
    //
    //  Bit  Face      Constant
    //   0   minX      F_MINX
    //   1   maxX      F_MAXX
    //   2   minY      F_FLOOR
    //   3   maxY      F_CEIL
    //   4   minZ      F_MINZ
    //   5   maxZ      F_MAXZ
    void addZone(const Vector3D& mn, const Vector3D& mx, int openFaces = 0);

    // Generates up to 6 box-mesh wall panels for one zone, skipping open
    // faces.  Panel ends are extended by thickness T only at CLOSED faces
    // (where a wall cap covers the corner).  Open-face ends are flush with
    // the zone boundary so adjacent panels never overlap — preventing the
    // Z-fighting seams that would otherwise appear at corridor junctions.
    void addSegWalls(const Vector3D& mn, const Vector3D& mx,
                     int openFaces, float T,
                     const wchar_t* wallTex, const wchar_t* floorTex);

    // Places a single scaled box-mesh panel at the given world position.
    // Uses CullMode::Front because box normals point outward — from inside
    // the corridor the visible surface is the box's back face.
    void addPanel(const Vector3D& centre, const Vector3D& scale,
                  const wchar_t* texPath = L"../Assets/Textures/wall.jpg");

    // Spawns one asteroid entity at pos with uniform scale.
    // Collision radius is derived from scale after creation.
    Asteroid* spawnAsteroid(const Vector3D& pos, float scale);

    // ---- Per-frame collision detection ----------------------------------

    void checkCollisions();

    // Squared distance between two points (avoids a sqrt for comparisons).
    static float distSq(const Vector3D& a, const Vector3D& b)
    {
        float dx = a.m_x - b.m_x, dy = a.m_y - b.m_y, dz = a.m_z - b.m_z;
        return dx*dx + dy*dy + dz*dz;
    }
};
