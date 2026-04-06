#pragma once
#include "../All.h"
#include "Ship.h"
#include "Asteroid.h"
#include "Projectile.h"
#include "Missile.h"
#include <vector>
#include <algorithm>

struct TunnelZone
{
    Vector3D min, max;

    bool containsSphere(const Vector3D& c, float r) const
    {
        return c.m_x>=min.m_x+r && c.m_x<=max.m_x-r
            && c.m_y>=min.m_y+r && c.m_y<=max.m_y-r
            && c.m_z>=min.m_z+r && c.m_z<=max.m_z-r;
    }
    Vector3D clampSphere(const Vector3D& c, float r) const
    {
        auto cl=[](float v,float lo,float hi){ return v<lo?lo:(v>hi?hi:v); };
        return {cl(c.m_x,min.m_x+r,max.m_x-r),
                cl(c.m_y,min.m_y+r,max.m_y-r),
                cl(c.m_z,min.m_z+r,max.m_z-r)};
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
    bool  m_input = true;
    Ship* m_ship  = nullptr;

    std::vector<TunnelZone> m_zones;

    void buildMineLevel();

    // Register a collision zone (extends by Ship::kShipRadius at open faces).
    // openFaces bits: 0=minX 1=maxX 2=minY(floor) 3=maxY(ceil) 4=minZ 5=maxZ
    void addZone(const Vector3D& mn, const Vector3D& mx, int openFaces = 0);

    // Generate wall panels for all 6 faces of a box, skipping open faces.
    void addSegWalls(const Vector3D& mn, const Vector3D& mx,
                     int openFaces, float T,
                     const wchar_t* wallTex, const wchar_t* floorTex);

    // Place a single scaled box-mesh wall panel.
    void addPanel(const Vector3D& center, const Vector3D& scale,
                  const wchar_t* texPath = L"../Assets/Textures/wall.jpg");

    Asteroid* spawnAsteroid(const Vector3D& pos, float scale);

    void checkCollisions();
    static float distSq(const Vector3D& a, const Vector3D& b)
    {
        float dx=a.m_x-b.m_x, dy=a.m_y-b.m_y, dz=a.m_z-b.m_z;
        return dx*dx+dy*dy+dz*dz;
    }
};
