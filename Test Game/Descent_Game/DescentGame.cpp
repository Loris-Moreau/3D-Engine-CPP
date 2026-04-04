#include "DescentGame.h"

#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <cmath>

// =========================================================================
//  Helpers — inline lambdas
// =========================================================================
static float randF(float lo, float hi)
{
    return lo + (float)(rand() % 10000) / 10000.0f * (hi - lo);
}

// =========================================================================
//  onCreate
// =========================================================================
void DescentGame::onCreate()
{
    setTitle(L"Descent – Mines of Zeta Aquilae");
    srand((unsigned int)time(nullptr));

    // Stars skybox
    {
        auto tex  = createTexture(L"../Assets/Textures/stars_map.jpg");
        auto mesh = createMesh(L"../Assets/Meshes/sphere.obj");
        auto mat  = createMaterial(L"../Assets/Shaders/SkyBox.hlsl"); // FIX: was skybox.hlsl
        mat->addTexture(tex);
        mat->setCullMode(CullMode::Front);

        auto sky = createEntity<MeshEntity>();
        sky->setMesh(mesh);
        sky->addMaterial(mat);
        sky->setScale(Vector3D(20000, 20000, 20000));
    }

    buildMineLevel();

    // Spawn player ship at the south entrance of the mine, facing the corridor.
    // FIX: was Z=2700 facing +Z -- pressing forward immediately hit the back wall.
    // Now spawns at Z=2500 facing -Z (toward the main corridor at Z<2000).
    m_ship = createEntity<Ship>();
    if (m_ship)
    {
        m_ship->setPosition(Vector3D(0, 0, 2500));
        m_ship->setInitialYaw(3.14159265f); // face toward corridor (-Z direction)
    }

    getInputManager()->enablePlayMode(m_input);
}

// =========================================================================
//  onUpdate
// =========================================================================
void DescentGame::onUpdate(float /*dt*/)
{
    // Toggle cursor lock with Escape
    if (getInputManager()->isKeyUp(Key::Escape))
    {
        m_input = !m_input;
        getInputManager()->enablePlayMode(m_input);
    }

    checkCollisions();
}

// =========================================================================
//  constrainPosition  (Game override)
//
//  Returns the nearest legal position for a sphere of radius `radius`.
//  If the sphere centre is already inside any zone it is returned unchanged.
//  Otherwise we project it to the closest point on the inset boundary of
//  the nearest zone, so the ship slides along walls rather than clipping.
// =========================================================================
Vector3D DescentGame::constrainPosition(const Vector3D& pos, float radius) const
{
    // Fast path: already inside a zone
    for (const auto& z : m_zones)
        if (z.containsSphere(pos, radius)) return pos;

    // Find the zone whose clamped point is closest to pos
    float    bestDist = 1e30f;
    Vector3D bestPos  = pos;

    for (const auto& z : m_zones)
    {
        Vector3D clamped = z.clampSphere(pos, radius);
        float dx = clamped.m_x - pos.m_x;
        float dy = clamped.m_y - pos.m_y;
        float dz = clamped.m_z - pos.m_z;
        float d  = dx*dx + dy*dy + dz*dz;
        if (d < bestDist) { bestDist = d; bestPos = clamped; }
    }

    return bestPos;
}

// =========================================================================
//  checkCollisions
// =========================================================================
void DescentGame::checkCollisions()
{
    // Remove dead asteroids from our tracking list
    m_asteroids.erase(
        std::remove_if(m_asteroids.begin(), m_asteroids.end(),
            [](Asteroid* a){ return a == nullptr; }),
        m_asteroids.end());

    auto projectiles = getEntitiesOfType<Projectile>();
    auto missiles    = getEntitiesOfType<Missile>();

    for (Asteroid* asteroid : m_asteroids)
    {
        if (!asteroid) continue;
        float ar  = asteroid->getCollisionRadius();
        auto  ap  = asteroid->getPosition();

        // --- Projectile vs Asteroid ---
        for (Projectile* proj : projectiles)
        {
            float rSum = ar + proj->getCollisionRadius();
            if (distSq(proj->getPosition(), ap) < rSum * rSum)
            {
                asteroid->TakeDamage(proj->GetDamage());
                proj->release();
                // Nullify in our list if asteroid was destroyed
                if (asteroid->m_health <= 0.0f) { asteroid = nullptr; break; }
            }
        }
        if (!asteroid) continue;

        // --- Missile vs Asteroid ---
        for (Missile* mis : missiles)
        {
            float rSum = ar + mis->getCollisionRadius();
            if (distSq(mis->getPosition(), ap) < rSum * rSum)
            {
                asteroid->TakeDamage(mis->GetDamage());
                mis->release();
                if (asteroid->m_health <= 0.0f) { asteroid = nullptr; break; }
            }
        }
        if (!asteroid) continue;

        // --- Ship vs Asteroid (ramming) ---
        if (m_ship)
        {
            float rSum = ar + Ship::kShipRadius;
            if (distSq(m_ship->getPosition(), ap) < rSum * rSum)
            {
                m_ship->TakeDamage(10.0f); // ramming costs 10 hp per hit
            }
        }
    }
}

// =========================================================================
//  buildMineLevel
//
//  Layout (top-down view, Y = height):
//
//       [NW room]──[N corridor]──[N room]──[N corridor]──[NE room]
//                                   |
//                             [vert shaft]
//                                   |
//       [SW room]──[S corridor]──[center]──[S corridor]──[SE room]
//                                   |
//                            [S start room]
//
//  All distances in engine units.  The ship starts in the south start room.
// =========================================================================
void DescentGame::buildMineLevel()
{
    // ---- Lights -----------------------------------------------------------
    // Main white directional
    {
        auto light = createEntity<LightEntity>();
        light->setColor(Vector3D(0.9f, 0.85f, 0.75f));
        light->setRotation(Vector3D(-0.4f, 0.6f, 0.0f));
    }
    // Dim red fill
    {
        auto light = createEntity<LightEntity>();
        light->setColor(Vector3D(0.5f, 0.05f, 0.05f));
        light->setRotation(Vector3D(0.6f, -0.6f, 0.0f));
    }

    // ----------------------------------------------------------------
    //  Corridor & room constants
    // ----------------------------------------------------------------
    // Interior half-extents for corridors and rooms
    constexpr float cHX = 200.0f; // corridor half-width  (X)
    constexpr float cHY = 130.0f; // corridor half-height (Y)
    constexpr float cHZ = 200.0f; // corridor half-depth  (Z, for cross arm)
    constexpr float cLen = 2000.0f; // corridor half-length along its travel axis

    constexpr float rHX = 400.0f; // room half-size X
    constexpr float rHY = 260.0f; // room half-size Y
    constexpr float rHZ = 400.0f; // room half-size Z

    // ----------------------------------------------------------------
    //  Define passable zones
    //  Each zone uses addZone(min, max, openFaces).
    //  openFaces bits: 0=minX 1=maxX 2=minY 3=maxY 4=minZ 5=maxZ
    //  Open face = no wall panel on that face (it connects to another zone).
    // ----------------------------------------------------------------

    // Main N-S corridor  (runs along Z, centred on X=0)
    // Open: minZ(4) and maxZ(5) — both ends connect to rooms
    addZone(
        Vector3D(-cHX, -cHY, -cLen),
        Vector3D( cHX,  cHY,  cLen),
        (1<<4)|(1<<5) );   // open both Z ends

    // Cross E-W corridor  (runs along X, centred on Z=0)
    // Open: minX(0) and maxX(1) — both ends connect to rooms
    // Also open minZ(4) and maxZ(5) are not needed here (narrow Z extent)
    addZone(
        Vector3D(-cLen, -cHY, -cHZ),
        Vector3D( cLen,  cHY,  cHZ),
        (1<<0)|(1<<1) );

    // South start room  (Z > cLen)
    // Open: minZ face (bit4) — connects north to main corridor
    addZone(
        Vector3D(-rHX, -rHY,  cLen),
        Vector3D( rHX,  rHY,  cLen + rHZ*2),
        (1<<4) );

    // North dead-end room  (Z < -cLen)
    // Open: maxZ face (bit5) — connects south to main corridor
    addZone(
        Vector3D(-rHX, -rHY, -cLen - rHZ*2),
        Vector3D( rHX,  rHY, -cLen),
        (1<<5) );

    // East room  (X > cLen)
    // Open: minX face (bit0) — connects west to cross corridor
    addZone(
        Vector3D( cLen,      -rHY, -rHZ),
        Vector3D( cLen+rHX*2, rHY,  rHZ),
        (1<<0) );

    // West room  (X < -cLen)
    // Open: maxX face (bit1) — connects east to cross corridor
    addZone(
        Vector3D(-cLen-rHX*2, -rHY, -rHZ),
        Vector3D(-cLen,        rHY,  rHZ),
        (1<<1) );

    // ----------------------------------------------------------------
    //  Decor: small alcoves branching off the main corridor
    // ----------------------------------------------------------------
    // Two alcoves on the east wall at different Z positions
    addZone(
        Vector3D( cHX,       -80.0f,  600.0f),
        Vector3D( cHX+200.0f, 80.0f,  900.0f),
        (1<<0) );  // open minX (connects to corridor)
    addZone(
        Vector3D( cHX,       -80.0f, -900.0f),
        Vector3D( cHX+200.0f, 80.0f, -600.0f),
        (1<<0) );

    // ----------------------------------------------------------------
    //  Asteroids / obstacles
    // ----------------------------------------------------------------
    // Scatter some in each room and along the corridors
    auto spawnCluster = [&](const Vector3D& centre, int count,
                            float spreadX, float spreadY, float spreadZ,
                            float scaleMin, float scaleMax)
    {
        for (int i = 0; i < count; ++i)
        {
            Vector3D pos(
                centre.m_x + randF(-spreadX, spreadX),
                centre.m_y + randF(-spreadY, spreadY),
                centre.m_z + randF(-spreadZ, spreadZ)
            );
            float sc = randF(scaleMin, scaleMax);
            spawnAsteroid(pos, sc);
        }
    };

    spawnCluster(Vector3D(   0, 0, 2200), 6,  180, 100, 200, 2.0f, 7.0f); // south start room -- ahead of player
    spawnCluster(Vector3D(   0, 0,-2800), 8,  280, 180, 280, 3.0f, 8.0f); // north room
    spawnCluster(Vector3D( 2600, 0,   0), 7,  250, 180, 280, 2.0f, 6.0f); // east room
    spawnCluster(Vector3D(-2600, 0,   0), 7,  250, 180, 280, 2.0f, 6.0f); // west room
    spawnCluster(Vector3D(   0, 0,  1000), 4, 120,  80, 600, 1.5f, 4.0f); // mid corridor N
    spawnCluster(Vector3D(   0, 0, -1000), 4, 120,  80, 600, 1.5f, 4.0f); // mid corridor S
    spawnCluster(Vector3D( 1200, 0,   0), 4, 600,  80, 120, 1.5f, 4.0f); // cross arm E
    spawnCluster(Vector3D(-1200, 0,   0), 4, 600,  80, 120, 1.5f, 4.0f); // cross arm W
}

// =========================================================================
//  addZone — register passable volume and build its wall panels
// =========================================================================
void DescentGame::addZone(const Vector3D& mn, const Vector3D& mx, int openFaces)
{
    m_zones.push_back({ mn, mx });

    const float T = 20.0f; // wall thickness

    float cx = (mn.m_x + mx.m_x) * 0.5f;
    float cy = (mn.m_y + mx.m_y) * 0.5f;
    float cz = (mn.m_z + mx.m_z) * 0.5f;
    float sx = mx.m_x - mn.m_x;
    float sy = mx.m_y - mn.m_y;
    float sz = mx.m_z - mn.m_z;

    // Walls are placed flush with the zone boundary; we grow them by T outward
    // so they cover the solid side.  For interior joins we rely on zones
    // sharing the face (openFaces skips the panel entirely for joined faces).

    const wchar_t* wallTex  = L"../Assets/Textures/wall.jpg";
    const wchar_t* floorTex = L"../Assets/Textures/brick.png";

    // ---- Floor (minY) ----
    if (!(openFaces & (1<<2)))
        addWall(Vector3D(cx, mn.m_y - T*0.5f, cz),
                Vector3D(sx + T*2, T, sz + T*2), floorTex);

    // ---- Ceiling (maxY) ----
    if (!(openFaces & (1<<3)))
        addWall(Vector3D(cx, mx.m_y + T*0.5f, cz),
                Vector3D(sx + T*2, T, sz + T*2), floorTex);

    // ---- Left wall (minX) ----
    if (!(openFaces & (1<<0)))
        addWall(Vector3D(mn.m_x - T*0.5f, cy, cz),
                Vector3D(T, sy, sz + T*2), wallTex);

    // ---- Right wall (maxX) ----
    if (!(openFaces & (1<<1)))
        addWall(Vector3D(mx.m_x + T*0.5f, cy, cz),
                Vector3D(T, sy, sz + T*2), wallTex);

    // ---- Near wall (minZ) ----
    if (!(openFaces & (1<<4)))
        addWall(Vector3D(cx, cy, mn.m_z - T*0.5f),
                Vector3D(sx + T*2, sy, T), wallTex);

    // ---- Far wall (maxZ) ----
    if (!(openFaces & (1<<5)))
        addWall(Vector3D(cx, cy, mx.m_z + T*0.5f),
                Vector3D(sx + T*2, sy, T), wallTex);
}

// =========================================================================
//  addWall — spawn a scaled box-mesh wall panel entity
// =========================================================================
void DescentGame::addWall(const Vector3D& centre, const Vector3D& scale,
                          const wchar_t* texPath)
{
    auto tex  = createTexture(texPath);
    auto mesh = createMesh(L"../Assets/Meshes/box.obj");
    auto mat  = createMaterial(L"../Assets/Shaders/base.hlsl");
    mat->addTexture(tex);

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
    // Update collision radius now that scale is known
    a->setCollisionRadius(scale * 5.5f);
    a->setRotation(Vector3D(randF(0, 6.28f), randF(0, 6.28f), randF(0, 6.28f)));

    m_asteroids.push_back(a);
    return a;
}
