/*
 * DescentGame.cpp
 *
 * Restart on death:
 *   Ship::IsDead() is checked every frame. 
 *   restartLevel() calls Game::clearAllEntities() then onCreate() for a
 *   clean rebuild without leaking any entity memory.
 */

#include "DescentGame.h"

#include <cmath>
#include <cstdlib>
#include <ctime>

// Face-open bitmask constants.
// NEVER use bare integer literals — "4|5" equals decimal 5, NOT bit4|bit5.
static constexpr int F_MINX  = (1 << 0);
static constexpr int F_MAXX  = (1 << 1);
static constexpr int F_FLOOR = (1 << 2);
static constexpr int F_CEIL  = (1 << 3);
static constexpr int F_MINZ  = (1 << 4);
static constexpr int F_MAXZ  = (1 << 5);

static float randF(float lo, float hi)
{
    return lo + (rand() % 10000) / 10000.f * (hi - lo);
}

// ============================================================================
//  onCreate
// ============================================================================
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
        sky->setMesh(mesh); sky->addMaterial(mat);
        sky->setScale(Vector3D(20000, 20000, 20000));
    }

    buildMineLevel();

    // Ship must be spawned AFTER buildMineLevel so m_zones is populated.
    m_ship = createEntity<Ship>();
    if (m_ship)
    {
        m_ship->setPosition(Vector3D(0, 0, 2300));
        m_ship->setInitialYaw(3.14159265f);   // face -Z toward corridor
    }

    m_restartPending = false;
    m_restartTimer   = 0.f;

    getInputManager()->enablePlayMode(m_input);
}

// ============================================================================
//  restartLevel
//  Clears every entity and re-runs onCreate() with a clean slate.
//  Called from onUpdate() which runs before the per-entity update loop,
//  so no iterators are active during the clear.
// ============================================================================
void DescentGame::restartLevel()
{
    clearAllEntities();
    m_ship = nullptr;
    m_zones.clear();
    onCreate();
    // FIX: the new camera has a zero-size screen area until this is called.
    // Without it, setPerspectiveFovLH receives 0/0 aspect ratio, writing
    // NaN into the projection matrix -- resulting in a black screen.
    // updateCamerasViewportSize() is now protected in Game so we can call it.
    updateCamerasViewportSize();
}

// ============================================================================
//  onUpdate
// ============================================================================
void DescentGame::onUpdate(float dt)
{
    // --- Death / restart ---
    if (m_restartPending)
    {
        m_restartTimer -= dt;
        if (m_restartTimer <= 0.f) restartLevel();
        return;   // skip all other logic while restarting
    }

    if (m_ship && m_ship->IsDead())
    {
        m_restartPending = true;
        m_restartTimer   = kRestartDelay;
        return;
    }

    // --- Escape toggles cursor lock ---
    if (getInputManager()->isKeyUp(Key::Escape))
    {
        m_input = !m_input;
        getInputManager()->enablePlayMode(m_input);
    }

    checkCollisions();
}

// ============================================================================
//  constrainPosition
// ============================================================================
Vector3D DescentGame::constrainPosition(const Vector3D& p, float r) const
{
    for (const auto& z : m_zones)
        if (z.containsSphere(p, r)) return p;

    float best = 1e30f;
    Vector3D res = p;
    for (const auto& z : m_zones)
    {
        Vector3D c  = z.clampSphere(p, r);
        float dx=c.m_x-p.m_x, dy=c.m_y-p.m_y, dz=c.m_z-p.m_z;
        float d = dx*dx+dy*dy+dz*dz;
        if (d < best) { best = d; res = c; }
    }
    return res;
}

// ============================================================================
//  isInBounds
// ============================================================================
bool DescentGame::isInBounds(const Vector3D& p, float r) const
{
    for (const auto& z : m_zones)
        if (z.containsSphere(p, r)) return true;
    return false;
}

// ============================================================================
//  checkCollisions
// ============================================================================
void DescentGame::checkCollisions()
{
    auto asteroids   = getEntitiesOfType<Asteroid>();
    auto enemies     = getEntitiesOfType<Enemy>();
    auto projectiles = getEntitiesOfType<Projectile>();
    auto missiles    = getEntitiesOfType<Missile>();
    auto enemyBolts  = getEntitiesOfType<EnemyProjectile>();

    // --- Player weapons vs asteroids ---
    for (Asteroid* a : asteroids)
    {
        if (!a || a->m_health <= 0.f) continue;
        float ar = a->getCollisionRadius();
        auto  ap = a->getPosition();
        for (Projectile* p : projectiles)
        {
            float rs = ar + p->getCollisionRadius();
            if (distSq(p->getPosition(), ap) < rs*rs)
            { a->TakeDamage(p->GetDamage()); p->release(); if(a->m_health<=0.f) goto na; }
        }
        for (Missile* m : missiles)
        {
            float rs = ar + m->getCollisionRadius();
            if (distSq(m->getPosition(), ap) < rs*rs)
            { a->TakeDamage(m->GetDamage()); m->release(); if(a->m_health<=0.f) goto na; }
        }
        if (m_ship)
        {
            float rs = ar + Ship::kShipRadius;
            if (distSq(m_ship->getPosition(), ap) < rs*rs)
                m_ship->TakeDamage(10.f);   // ramming hurts the ship
        }
        na:;
    }

    // --- Player weapons vs enemies ---
    for (Enemy* e : enemies)
    {
        if (!e || e->m_health <= 0.f) continue;
        float er = e->getCollisionRadius();
        auto  ep = e->getPosition();
        for (Projectile* p : projectiles)
        {
            float rs = er + p->getCollisionRadius();
            if (distSq(p->getPosition(), ep) < rs*rs)
            { e->TakeDamage(p->GetDamage()); p->release(); if(e->m_health<=0.f) goto ne; }
        }
        for (Missile* m : missiles)
        {
            float rs = er + m->getCollisionRadius();
            if (distSq(m->getPosition(), ep) < rs*rs)
            { e->TakeDamage(m->GetDamage()); m->release(); if(e->m_health<=0.f) goto ne; }
        }
        ne:;
    }

    // --- Enemy bolts vs player ship ---
    if (m_ship)
    {
        for (EnemyProjectile* bolt : enemyBolts)
        {
            if (!bolt) continue;
            float rs = Ship::kShipRadius + bolt->getCollisionRadius();
            if (distSq(bolt->getPosition(), m_ship->getPosition()) < rs*rs)
            {
                m_ship->TakeDamage(bolt->GetDamage());
                bolt->release();
            }
        }
    }
}

// ============================================================================
//  buildMineLevel
// ============================================================================
void DescentGame::buildMineLevel()
{
    { auto l=createEntity<LightEntity>(); l->setColor({0.9f,0.85f,0.75f}); l->setRotation({-0.4f, 0.6f,0}); }
    { auto l=createEntity<LightEntity>(); l->setColor({0.5f,0.05f,0.05f}); l->setRotation({ 0.6f,-0.6f,0}); }

    // Shared dimensions — all corridors use the same W×H cross-section
    // so panels join perfectly at every junction without frame panels.
    const float W  = 200.f;   // corridor half-width  → total 400 u
    const float H  = 130.f;   // corridor half-height → total 260 u
    const float L  = 2000.f;  // arm half-length from center
    const float RD = 400.f;   // room extra depth past arm end
    const float SX = 1600.f;  // X where east shaft begins / west shaft ends
    const float SY = 700.f;   // shaft top Y (east) / shaft bottom Y mirror (west)
    const float RY = 450.f;   // upper east room floor Y
    const float RX = 2400.f;  // upper east room east wall X
    const float T  = 25.f;    // wall panel thickness
    const wchar_t* WT = L"../Assets/Textures/wall.jpg";
    const wchar_t* FT = L"../Assets/Textures/brick.png";

    // ---- COLLISION ZONES ------------------------------------------------
    addZone({-W,-H,-W},{W,H,W}, F_MINX|F_MAXX|F_MINZ|F_MAXZ); // center junction

    // N–S corridor
    addZone({-W,-H,  W},{W,H,  L},  F_MINZ|F_MAXZ);  // south arm
    addZone({-W,-H,  L},{W,H,L+RD}, F_MINZ);          // south start room
    addZone({-W,-H, -L},{W,H, -W},  F_MINZ|F_MAXZ);  // north arm
    addZone({-W,-H,-L-RD},{W,H,-L}, F_MAXZ);          // north room

    // E–W corridor
    addZone({-L,-H,-W},{-W, H,W},  F_MINX|F_MAXX);   // west arm (non-shaft part)
    addZone({ W,-H,-W},{SX, H,W},  F_MINX|F_MAXX);   // east arm (non-shaft part)
    addZone({SX,-H,-W},{ L, H,W},  F_MINX|F_MAXX|F_CEIL);  // east arm over shaft
    addZone({SX, H,-W},{ L,SY,W},  F_MINX|F_MAXX|F_FLOOR); // east shaft vertical
    addZone({ L,RY,-W},{RX,SY,W},  F_MINX);           // upper east room

    // West shaft (going DOWN)
    addZone({-L,-H,-W},{-SX,H,  W}, F_MINX|F_MAXX|F_FLOOR); // west arm over shaft
    addZone({-L,-SY,-W},{-SX,-H,W}, F_MINX|F_MAXX|F_CEIL);  // west shaft vertical
    addZone({-RX,-SY,-W},{-L,-RY,W},F_MAXX);          // lower west room

    // ---- VISUAL WALLS ---------------------------------------------------
    addSegWalls({-W,-H,-W},{W,H,W}, F_MINX|F_MAXX|F_MINZ|F_MAXZ, T,WT,FT); // center (floor+ceil only)

    // Center junction side walls.
    // addSegWalls skips all 4 lateral faces on the junction (they are marked open).
    // The arm side walls only cover their own Z/X range, leaving a visible gap
    // in the 400x400 crossroads area. These 4 panels close that gap.
    // Z-ends are flush (no T extension) so they abut the arm panels at z=+/-W exactly.
    addPanel({-W-T/2.f, 0.f,      0.f}, {T,     2.f*H, 2.f*W}, WT); // left  (-X)
    addPanel({ W+T/2.f, 0.f,      0.f}, {T,     2.f*H, 2.f*W}, WT); // right (+X)
    addPanel({     0.f, 0.f, -W-T/2.f}, {2.f*W, 2.f*H, T    }, WT); // front (-Z)
    addPanel({     0.f, 0.f,  W+T/2.f}, {2.f*W, 2.f*H, T    }, WT); // back  (+Z)

    // N–S
    addSegWalls({-W,-H, W},{W,H, L},  F_MINZ|F_MAXZ, T,WT,FT);
    addSegWalls({-W,-H, L},{W,H,L+RD},F_MINZ,        T,WT,FT);
    addSegWalls({-W,-H,-L},{W,H,-W},  F_MINZ|F_MAXZ, T,WT,FT);
    addSegWalls({-W,-H,-L-RD},{W,H,-L},F_MAXZ,       T,WT,FT);

    // E–W non-shaft parts
    // BUG FIX: west arm addSegWalls only covers -SX to -W now (not -L to -W).
    //   The shaft section (-L to -SX) is handled by manual panels below so
    //   there are no duplicate floor/ceiling panels causing Z-fighting seams.
    addSegWalls({-SX,-H,-W},{-W,H,W}, F_MINX|F_MAXX, T,WT,FT);  // west arm (non-shaft)
    addSegWalls({ W,-H,-W},{SX,H, W}, F_MINX|F_MAXX, T,WT,FT);  // east arm (non-shaft)

    // ---- EAST SHAFT PANELS (SX to L) ------------------------------------
    // West face (at X=SX) is OPEN — connects to the east arm.
    //   → x0 = SX  (flush, no T extension on west side)
    //   → x1 = L+T (closed east face, extends by T)
    //   → size_x = L - SX + T
    //   → centre_x = (SX + L + T) / 2
    // This is the key fix: previously x0 was SX-T, overlapping the arm by T.
    {
        const float cx  = (SX + L + T) / 2.f;  // 1812.5
        const float sx  = L - SX + T;           // 425
        const float sz  = 2.f*W + 2.f*T;        // 450   (Z: both sides closed)

        addPanel({cx,      -H - T/2,      0}, {sx, T,    sz}, FT); // arm floor
        addPanel({cx,      SY + T/2,      0}, {sx, T,    sz}, FT); // shaft top ceiling
        addPanel({cx,       H + T/2,      0}, {sx, T,    sz}, FT); // arm ceiling patch
        addPanel({cx, (-H + SY) / 2, -W-T/2}, {sx, SY+H, T}, WT); // front wall (-Z)
        addPanel({cx, (-H + SY) / 2,  W+T/2}, {sx, SY+H, T}, WT); // back wall  (+Z)
        // Shaft west wall: above arm ceiling (Y=H to Y=SY), only T wide in X
        addPanel({SX - T/2, (H + SY) / 2,        0}, {T, SY-H, sz}, WT);
        // East wall: below upper room floor (Y=-H to Y=RY), at X=L
        addPanel({L  + T/2, (-H + RY) / 2,        0}, {T, RY+H, sz}, WT);
    }
    addSegWalls({L,RY,-W},{RX,SY,W}, F_MINX, T,WT,FT); // upper east room

    // ---- WEST SHAFT PANELS (-L to -SX) ----------------------------------
    // East face (at X=-SX) is OPEN — connects to the west arm.
    //   → x1 = -SX  (flush)
    //   → x0 = -L-T (closed west face)
    //   → size_x = L - SX + T  (same as east shaft by symmetry)
    //   → centre_x = -(L + T + SX) / 2
    {
        const float wsY = -SY;
        const float rY  = -RY;
        const float cx  = -(L + T + SX) / 2.f;  // -1812.5
        const float sx  = L - SX + T;            // 425
        const float sz  = 2.f*W + 2.f*T;         // 450

        addPanel({cx,          H + T/2,       0}, {sx, T,      sz}, FT); // arm ceiling
        addPanel({cx,      wsY - T/2,          0}, {sx, T,      sz}, FT); // shaft floor
        addPanel({cx,         -H - T/2,        0}, {sx, T,      sz}, FT); // arm floor patch
        addPanel({cx, (-H + wsY) / 2, -W-T/2}, {sx, -H-wsY,  T}, WT); // front wall
        addPanel({cx, (-H + wsY) / 2,  W+T/2}, {sx, -H-wsY,  T}, WT); // back wall
        // Shaft east wall: below arm floor (Y=wsY to Y=-H), at X=-SX
        addPanel({-SX + T/2, (wsY - H) / 2,       0}, {T, -H-wsY, sz}, WT);
        // West wall: above lower room ceiling (Y=wsY to Y=rY), at X=-L
        addPanel({-L  - T/2, (wsY + rY) / 2,       0}, {T, rY-wsY, sz}, WT);
    }
    addSegWalls({-RX,-SY,-W},{-L,-RY,W}, F_MAXX, T,WT,FT); // lower west room

    // ---- ASTEROIDS -------------------------------------------------------
    auto cluster = [&](Vector3D c, int n, float sx, float sy, float sz,
                       float sMin, float sMax)
    {
        for (int i=0; i<n; i++)
            spawnAsteroid(
                {c.m_x+randF(-sx,sx), c.m_y+randF(-sy,sy), c.m_z+randF(-sz,sz)},
                randF(sMin, sMax));
    };

    cluster({   0,   0,  2200}, 3, 140, 80, 140, 2.f, 5.f); // south room
    cluster({   0,   0, -2300}, 5, 150, 80, 150, 2.f, 6.f); // north room
    cluster({   0,   0,   900}, 2, 120, 70, 400, 2.f, 4.f); // south arm
    cluster({   0,   0,  -900}, 2, 120, 70, 400, 2.f, 4.f); // north arm
    cluster({1200,   0,     0}, 2, 500, 70, 120, 2.f, 4.f); // east arm
    cluster({-1200,  0,     0}, 2, 500, 70, 120, 2.f, 4.f); // west arm
    cluster({2200,  550,    0}, 4, 140, 60, 140, 2.f, 5.f); // upper east room
    cluster({-2200,-550,    0}, 4, 140, 60, 140, 2.f, 5.f); // lower west room

    // ---- ENEMIES ---------------------------------------------------------
    // The south start room is enemy-free so the player can orient.
    // Enemies are spaced so the player encounters them steadily as they explore.
    // Each arm and room has at least one enemy; tight spaces have one,
    // open rooms have two so the player has to watch different angles.

    // South arm — first contact as soon as the player leaves the start room
    spawnEnemy({  0,   0,  1500});
    spawnEnemy({  0,   0,   600});

    // Center junction — guards the crossroads
    spawnEnemy({ 150, 0,    0});
    spawnEnemy({-150, 0,    0});

    // North arm
    spawnEnemy({  0,   0,  -700});
    spawnEnemy({  0,   0, -1500});

    // North room — two guards inside the dead-end room
    spawnEnemy({ 120,  0, -2200});
    spawnEnemy({-120,  0, -2400});

    // East arm
    spawnEnemy({ 700,  0,    0});
    spawnEnemy({1400,  0,    0});

    // Upper east room — hardest area, three enemies
    spawnEnemy({2100,  550,   80});
    spawnEnemy({2100,  550,  -80});
    spawnEnemy({2300,  480,    0});

    // West arm
    spawnEnemy({-700,   0,    0});
    spawnEnemy({-1400,  0,    0});

    // Lower west room — three enemies to match upper east
    spawnEnemy({-2100, -550,   80});
    spawnEnemy({-2100, -550,  -80});
    spawnEnemy({-2300, -480,    0});
}

// ============================================================================
//  addZone
// ============================================================================
void DescentGame::addZone(const Vector3D& mn, const Vector3D& mx, int open)
{
    const float R = Ship::kShipRadius;
    TunnelZone z; z.min=mn; z.max=mx;
    if (open&F_MINX)  z.min.m_x-=R;
    if (open&F_MAXX)  z.max.m_x+=R;
    if (open&F_FLOOR) z.min.m_y-=R;
    if (open&F_CEIL)  z.max.m_y+=R;
    if (open&F_MINZ)  z.min.m_z-=R;
    if (open&F_MAXZ)  z.max.m_z+=R;
    m_zones.push_back(z);
}

// ============================================================================
//  addSegWalls
//  Each panel extends by T only at CLOSED faces; open-face ends stop flush
//  with the zone boundary so no adjacent panels overlap.
// ============================================================================
void DescentGame::addSegWalls(const Vector3D& mn, const Vector3D& mx,
                               int open, float T,
                               const wchar_t* wt, const wchar_t* ft)
{
    const float x0 = (open&F_MINX)  ? mn.m_x : mn.m_x - T;
    const float x1 = (open&F_MAXX)  ? mx.m_x : mx.m_x + T;
    const float y0 = (open&F_FLOOR) ? mn.m_y : mn.m_y - T;
    const float y1 = (open&F_CEIL)  ? mx.m_y : mx.m_y + T;
    const float z0 = (open&F_MINZ)  ? mn.m_z : mn.m_z - T;
    const float z1 = (open&F_MAXZ)  ? mx.m_z : mx.m_z + T;

    const float fsx=x1-x0, fcx=(x0+x1)*.5f;
    const float fsz=z1-z0, fcz=(z0+z1)*.5f;
    const float wsy=y1-y0, wcy=(y0+y1)*.5f;

    if(!(open&F_FLOOR)) addPanel({fcx,mn.m_y-T*.5f,fcz},{fsx,T,  fsz},ft);
    if(!(open&F_CEIL))  addPanel({fcx,mx.m_y+T*.5f,fcz},{fsx,T,  fsz},ft);
    if(!(open&F_MINX))  addPanel({mn.m_x-T*.5f,wcy,fcz},{T,  wsy,fsz},wt);
    if(!(open&F_MAXX))  addPanel({mx.m_x+T*.5f,wcy,fcz},{T,  wsy,fsz},wt);
    if(!(open&F_MINZ))  addPanel({fcx,wcy,mn.m_z-T*.5f},{fsx,wsy,T  },wt);
    if(!(open&F_MAXZ))  addPanel({fcx,wcy,mx.m_z+T*.5f},{fsx,wsy,T  },wt);
}

// ============================================================================
//  addPanel
// ============================================================================
void DescentGame::addPanel(const Vector3D& c, const Vector3D& s, const wchar_t* tex)
{
    auto t    = createTexture(tex);
    auto mesh = createMesh(L"../Assets/Meshes/box.obj");
    auto mat  = createMaterial(L"../Assets/Shaders/base.hlsl");
    mat->addTexture(t);
    mat->setCullMode(CullMode::Front);
    auto e = createEntity<MeshEntity>();
    e->setMesh(mesh); e->addMaterial(mat);
    e->setPosition(c); e->setScale(s);
}

// ============================================================================
//  spawnAsteroid
// ============================================================================
Asteroid* DescentGame::spawnAsteroid(const Vector3D& pos, float scale)
{
    auto* a = createEntity<Asteroid>();
    if (!a) return nullptr;
    a->setPosition(pos);
    a->setScale({scale,scale,scale});
    a->setCollisionRadius(scale*5.5f);
    a->setRotation({randF(0,6.28f),randF(0,6.28f),randF(0,6.28f)});
    return a;
}

// ============================================================================
//  spawnEnemy
// ============================================================================
Enemy* DescentGame::spawnEnemy(const Vector3D& pos)
{
    auto* e = createEntity<Enemy>();
    if (e) e->setPosition(pos);
    return e;
}
