#include "DescentGame.h"

#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <cmath>

static float randF(float lo, float hi)
{
    return lo + (float)(rand() % 10000) / 10000.0f * (hi - lo);
}

// =========================================================================
//  onCreate
// =========================================================================
void DescentGame::onCreate()
{
    setTitle(L"Descent - Mines of Zeta Aquilae");
    srand((unsigned int)time(nullptr));

    // Skybox
    {
        auto tex  = createTexture(L"../Assets/Textures/stars_map.jpg");
        auto mesh = createMesh(L"../Assets/Meshes/sphere.obj");
        auto mat  = createMaterial(L"../Assets/Shaders/SkyBox.hlsl");
        mat->addTexture(tex);
        mat->setCullMode(CullMode::Front);
        auto sky = createEntity<MeshEntity>();
        sky->setMesh(mesh);
        sky->addMaterial(mat);
        sky->setScale(Vector3D(20000, 20000, 20000));
    }

    buildMineLevel();

    // Ship spawns in south start room facing the corridor (-Z direction)
    m_ship = createEntity<Ship>();
    if (m_ship)
    {
        m_ship->setPosition(Vector3D(0, 0, 2500));
        m_ship->setInitialYaw(3.14159265f);
    }

    getInputManager()->enablePlayMode(m_input);
}

// =========================================================================
//  onUpdate
// =========================================================================
void DescentGame::onUpdate(float /*dt*/)
{
    if (getInputManager()->isKeyUp(Key::Escape))
    {
        m_input = !m_input;
        getInputManager()->enablePlayMode(m_input);
    }

    checkCollisions();
}

// =========================================================================
//  constrainPosition
//  Keeps a sphere (pos, radius) inside the union of all tunnel zones.
//  Fast-path: already in a zone → return unchanged.
//  Otherwise:  project to the closest zone boundary so the ship slides
//              along walls instead of stopping dead.
// =========================================================================
Vector3D DescentGame::constrainPosition(const Vector3D& pos, float radius) const
{
    for (const auto& z : m_zones)
        if (z.containsSphere(pos, radius)) return pos;

    float    bestDist = 1e30f;
    Vector3D bestPos  = pos;
    for (const auto& z : m_zones)
    {
        Vector3D c  = z.clampSphere(pos, radius);
        float dx = c.m_x - pos.m_x, dy = c.m_y - pos.m_y, dz = c.m_z - pos.m_z;
        float d = dx*dx + dy*dy + dz*dz;
        if (d < bestDist) { bestDist = d; bestPos = c; }
    }
    return bestPos;
}

// =========================================================================
//  isInBounds
//  Used by projectiles: returns true when pos is inside any zone.
//  Radius is used the same way as in constrainPosition so bullets stop
//  exactly at the wall face.
// =========================================================================
bool DescentGame::isInBounds(const Vector3D& pos, float radius) const
{
    for (const auto& z : m_zones)
        if (z.containsSphere(pos, radius)) return true;
    return false;
}

// =========================================================================
//  checkCollisions
// =========================================================================
void DescentGame::checkCollisions()
{
    // FIX: use getEntitiesOfType every frame instead of a raw-pointer cache.
    // Raw pointers in m_asteroids became dangling after release() was called,
    // causing undefined behaviour on the next frame access.
    auto asteroids   = getEntitiesOfType<Asteroid>();
    auto projectiles = getEntitiesOfType<Projectile>();
    auto missiles    = getEntitiesOfType<Missile>();

    for (Asteroid* asteroid : asteroids)
    {
        if (!asteroid || asteroid->m_health <= 0.0f) continue;

        float ar = asteroid->getCollisionRadius();
        auto  ap = asteroid->getPosition();

        // Projectile vs Asteroid
        for (Projectile* proj : projectiles)
        {
            if (!proj) continue;
            float rSum = ar + proj->getCollisionRadius();
            if (distSq(proj->getPosition(), ap) < rSum * rSum)
            {
                asteroid->TakeDamage(proj->GetDamage());
                proj->release();
                if (asteroid->m_health <= 0.0f) goto next_asteroid;
            }
        }

        // Missile vs Asteroid
        for (Missile* mis : missiles)
        {
            if (!mis) continue;
            float rSum = ar + mis->getCollisionRadius();
            if (distSq(mis->getPosition(), ap) < rSum * rSum)
            {
                asteroid->TakeDamage(mis->GetDamage());
                mis->release();
                if (asteroid->m_health <= 0.0f) goto next_asteroid;
            }
        }

        // Ship vs Asteroid (ramming)
        if (m_ship)
        {
            float rSum = ar + Ship::kShipRadius;
            if (distSq(m_ship->getPosition(), ap) < rSum * rSum)
                m_ship->TakeDamage(10.0f);
        }

        next_asteroid:;
    }
}

// =========================================================================
//  buildMineLevel
//
//  Cross-shaped mine layout (top-down):
//
//         [N room]
//            |
//       [N arm]
//            |
// [W room]--[W arm]--[junction]--[E arm]--[E room]
//            |
//       [S arm]
//            |
//        [S room]  ← player spawns here
//
// =========================================================================
void DescentGame::buildMineLevel()
{
    // Lights
    {
        auto light = createEntity<LightEntity>();
        light->setColor(Vector3D(0.9f, 0.85f, 0.75f));
        light->setRotation(Vector3D(-0.4f, 0.6f, 0.0f));
    }
    {
        auto light = createEntity<LightEntity>();
        light->setColor(Vector3D(0.5f, 0.05f, 0.05f));
        light->setRotation(Vector3D(0.6f, -0.6f, 0.0f));
    }

    // ----------------------------------------------------------------
    //  Dimensions
    // ----------------------------------------------------------------
    constexpr float cHX  = 200.0f;  // N-S corridor half-width  (X)
    constexpr float cHY  = 130.0f;  // corridor half-height     (Y)
    constexpr float cHZ  = 200.0f;  // E-W corridor half-depth  (Z)
    constexpr float cLen = 2000.0f; // arm half-length

    constexpr float rHX  = 400.0f;  // room half-width  (X)
    constexpr float rHY  = 260.0f;  // room half-height (Y)
    constexpr float rHZ  = 400.0f;  // room half-depth  (Z)

    // ----------------------------------------------------------------
    //  Zones
    //  FIX: each zone is split at junctions so its wall panels never
    //  cross through a neighbouring corridor.  Adjacent zones that share
    //  an open face are automatically extended by Ship::kShipRadius in
    //  addZone() so they overlap — eliminating invisible-wall dead-gaps.
    //
    //  openFaces: bit0=minX  bit1=maxX  bit2=floor  bit3=ceil
    //             bit4=minZ  bit5=maxZ
    // ----------------------------------------------------------------

    // Center junction: floor + ceiling only, all 4 side faces open
    addZone(Vector3D(-cHX, -cHY, -cHZ), Vector3D( cHX,  cHY,  cHZ),
            (1<<0)|(1<<1)|(1<<4)|(1<<5));

    // N-S south arm: Z = +cHZ .. +cLen
    addZone(Vector3D(-cHX, -cHY,  cHZ), Vector3D( cHX,  cHY,  cLen),
            (1<<4)|(1<<5));

    // N-S north arm: Z = -cLen .. -cHZ
    addZone(Vector3D(-cHX, -cHY, -cLen), Vector3D( cHX,  cHY, -cHZ),
            (1<<4)|(1<<5));

    // E-W west arm: X = -cLen .. -cHX
    addZone(Vector3D(-cLen, -cHY, -cHZ), Vector3D(-cHX,  cHY,  cHZ),
            (1<<0)|(1<<1));

    // E-W east arm: X = +cHX .. +cLen
    addZone(Vector3D( cHX, -cHY, -cHZ), Vector3D( cLen,  cHY,  cHZ),
            (1<<0)|(1<<1));

    // South start room (player spawns here)
    addZone(Vector3D(-rHX, -rHY,  cLen), Vector3D( rHX,  rHY,  cLen + rHZ*2),
            (1<<4));

    // North room
    addZone(Vector3D(-rHX, -rHY, -cLen - rHZ*2), Vector3D( rHX, rHY, -cLen),
            (1<<5));

    // East room
    addZone(Vector3D( cLen,       -rHY, -rHZ), Vector3D( cLen + rHX*2,  rHY,  rHZ),
            (1<<0));

    // West room
    addZone(Vector3D(-cLen - rHX*2, -rHY, -rHZ), Vector3D(-cLen,  rHY,  rHZ),
            (1<<1));

    // Side alcoves on the east wall of the N-S arms
    addZone(Vector3D( cHX, -80.0f,  600.0f), Vector3D( cHX + 200.0f,  80.0f,  900.0f), (1<<0));
    addZone(Vector3D( cHX, -80.0f, -900.0f), Vector3D( cHX + 200.0f,  80.0f, -600.0f), (1<<0));

    // ----------------------------------------------------------------
    //  Asteroid clusters
    // ----------------------------------------------------------------
    auto cluster = [&](Vector3D centre, int n,
                       float sx, float sy, float sz,
                       float scMin, float scMax)
    {
        for (int i = 0; i < n; ++i)
        {
            Vector3D p(centre.m_x + randF(-sx, sx),
                       centre.m_y + randF(-sy, sy),
                       centre.m_z + randF(-sz, sz));
            spawnAsteroid(p, randF(scMin, scMax));
        }
    };

    cluster(Vector3D(   0, 0,  2200),  6, 180, 100, 200, 2.0f, 7.0f); // south room
    cluster(Vector3D(   0, 0, -2800),  8, 280, 180, 280, 3.0f, 8.0f); // north room
    cluster(Vector3D( 2600, 0,    0),  7, 250, 180, 280, 2.0f, 6.0f); // east room
    cluster(Vector3D(-2600, 0,    0),  7, 250, 180, 280, 2.0f, 6.0f); // west room
    cluster(Vector3D(   0, 0,  1000),  4, 120,  80, 500, 1.5f, 4.0f); // south arm
    cluster(Vector3D(   0, 0, -1000),  4, 120,  80, 500, 1.5f, 4.0f); // north arm
    cluster(Vector3D( 1200, 0,    0),  4, 500,  80, 120, 1.5f, 4.0f); // east arm
    cluster(Vector3D(-1200, 0,    0),  4, 500,  80, 120, 1.5f, 4.0f); // west arm
}

// =========================================================================
//  addZone
//
//  Registers the passable volume and generates wall panels.
//
//  KEY FIX: each open face causes the stored zone boundary to be pushed
//  outward by Ship::kShipRadius.  Adjacent open-faced zones therefore
//  overlap by 2*kShipRadius.  containsSphere() insets by kShipRadius from
//  each stored boundary, so at the meeting plane both zones simultaneously
//  accept the sphere — no dead gap, no invisible wall.
// =========================================================================
void DescentGame::addZone(const Vector3D& mn, const Vector3D& mx, int openFaces)
{
    const float R = Ship::kShipRadius; // overlap extension at open faces
    const float T = 20.0f;             // visual wall panel thickness

    // Build the stored (extended) zone
    TunnelZone z;
    z.min = mn;  z.max = mx;
    if (openFaces & (1<<0)) z.min.m_x -= R;
    if (openFaces & (1<<1)) z.max.m_x += R;
    if (openFaces & (1<<2)) z.min.m_y -= R;
    if (openFaces & (1<<3)) z.max.m_y += R;
    if (openFaces & (1<<4)) z.min.m_z -= R;
    if (openFaces & (1<<5)) z.max.m_z += R;
    m_zones.push_back(z);

    // Wall panel geometry is computed from the ORIGINAL (un-extended) mn/mx
    float cx = (mn.m_x + mx.m_x) * 0.5f;
    float cy = (mn.m_y + mx.m_y) * 0.5f;
    float cz = (mn.m_z + mx.m_z) * 0.5f;
    float sx = mx.m_x - mn.m_x;
    float sy = mx.m_y - mn.m_y;
    float sz = mx.m_z - mn.m_z;

    const wchar_t* wallTex  = L"../Assets/Textures/wall.jpg";
    const wchar_t* floorTex = L"../Assets/Textures/brick.png";

    // Floor (minY)
    if (!(openFaces & (1<<2)))
        addWall(Vector3D(cx, mn.m_y - T*0.5f, cz),
                Vector3D(sx + T*2, T, sz + T*2), floorTex);

    // Ceiling (maxY)
    if (!(openFaces & (1<<3)))
        addWall(Vector3D(cx, mx.m_y + T*0.5f, cz),
                Vector3D(sx + T*2, T, sz + T*2), floorTex);

    // Left wall (minX)
    if (!(openFaces & (1<<0)))
        addWall(Vector3D(mn.m_x - T*0.5f, cy, cz),
                Vector3D(T, sy, sz + T*2), wallTex);

    // Right wall (maxX)
    if (!(openFaces & (1<<1)))
        addWall(Vector3D(mx.m_x + T*0.5f, cy, cz),
                Vector3D(T, sy, sz + T*2), wallTex);

    // Near wall (minZ)
    if (!(openFaces & (1<<4)))
        addWall(Vector3D(cx, cy, mn.m_z - T*0.5f),
                Vector3D(sx + T*2, sy, T), wallTex);

    // Far wall (maxZ)
    if (!(openFaces & (1<<5)))
        addWall(Vector3D(cx, cy, mx.m_z + T*0.5f),
                Vector3D(sx + T*2, sy, T), wallTex);
}

// =========================================================================
//  addWall
// =========================================================================
void DescentGame::addWall(const Vector3D& centre, const Vector3D& scale,
                          const wchar_t* texPath)
{
    auto tex   = createTexture(texPath);
    auto mesh  = createMesh(L"../Assets/Meshes/box.obj");
    auto mat   = createMaterial(L"../Assets/Shaders/base.hlsl");
    mat->addTexture(tex);

    // Walls viewed from inside the tunnel need front-face culling because
    // the box normals point outward (away from the corridor interior).
    mat->setCullMode(CullMode::Front);

    auto panel = createEntity<MeshEntity>();
    panel->setMesh(mesh);
    panel->addMaterial(mat);
    panel->setPosition(centre);
    panel->setScale(scale);
}

// =========================================================================
//  spawnAsteroid
// =========================================================================
Asteroid* DescentGame::spawnAsteroid(const Vector3D& pos, float scale)
{
    auto* a = createEntity<Asteroid>();
    if (!a) return nullptr;
    a->setPosition(pos);
    a->setScale(Vector3D(scale, scale, scale));
    a->setCollisionRadius(scale * 5.5f);  // override after scale is set
    a->setRotation(Vector3D(randF(0, 6.28f), randF(0, 6.28f), randF(0, 6.28f)));
    // FIX: no longer stored in m_asteroids — getEntitiesOfType<Asteroid>()
    // is used each frame so there are never dangling pointers.
    return a;
}
