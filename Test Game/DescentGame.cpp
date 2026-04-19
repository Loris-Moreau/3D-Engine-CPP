/*
 * DescentGame.cpp
 *
 * Collision summary (per frame):
 *   Player laser  (Projectile, 25 dmg) vs Asteroid  (75 hp) — 3 hits to kill
 *   Player laser  (Projectile, 25 dmg) vs Enemy     (75 hp) — 3 hits to kill
 *   Player missile(Missile,    50 dmg) vs Asteroid  (75 hp) — 2 hits to kill
 *   Player missile(Missile,    50 dmg) vs Enemy     (75 hp) — 2 hits to kill
 *   Enemy bolt (EnemyProjectile,34dmg) vs Ship     (100 hp) — 3 hits to kill
 *   Asteroid  ramming Ship (10 dmg/hit, invincibility prevents stacking)
 *
 * Player death → restartLevel() clears all entities and calls onCreate().
 */
#include "DescentGame.h"
#include <ctime>
#include <cstdlib>
#include <cmath>

static float randF(float lo, float hi)
{
    return lo + (rand() % 10000) / 10000.f * (hi - lo);
}

// Named face-open constants for the openFaces bitmask.
// IMPORTANT: never use bare integer literals like "4|5" — that evaluates
// to decimal 5 (minX|floor), NOT bit4|bit5 (minZ|maxZ).
static constexpr int F_MINX  = (1 << 0);
static constexpr int F_MAXX  = (1 << 1);
static constexpr int F_FLOOR = (1 << 2);
static constexpr int F_CEIL  = (1 << 3);
static constexpr int F_MINZ  = (1 << 4);
static constexpr int F_MAXZ  = (1 << 5);

// ============================================================================
//  onCreate
// ============================================================================
void DescentGame::onCreate()
{
    setTitle(L"Descent in the mines");
    srand((unsigned int)time(nullptr));

    // Skybox — large inverted sphere with a star-map texture.
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

    // Spawn player ship at the south start room, facing -Z into the corridor.
    m_ship = createEntity<Ship>();
    if (m_ship)
    {
        m_ship->setPosition(Vector3D(0, 0, 2300));
        m_ship->setInitialYaw(3.14159265f);
    }

    m_restartPending = false;
    m_restartTimer   = 0.f;

    getInputManager()->enablePlayMode(m_input);
}

// ============================================================================
//  restartLevel — clears all entities and re-runs onCreate.
//  Called from onUpdate(), which runs before the per-entity update loop in
//  onUpdateInternal(), so no iterators are active during the clear.
// ============================================================================
void DescentGame::restartLevel()
{
    clearAllEntities();   // immediately frees every entity (Game::clearAllEntities)
    m_ship = nullptr;
    m_zones.clear();
    onCreate();           // rebuild the whole level fresh
}

// ============================================================================
//  onUpdate
// ============================================================================
void DescentGame::onUpdate(float dt)
{
    // --- Player death → queued level restart ---
    if (m_restartPending)
    {
        m_restartTimer -= dt;
        if (m_restartTimer <= 0.f)
            restartLevel();   // clears + recreates everything; returns immediately
        return;               // skip collision this frame while restarting
    }

    // Check if the ship was just killed (health hit zero this frame).
    if (m_ship && m_ship->IsDead())
    {
        m_restartPending = true;
        m_restartTimer   = 1.5f;   // 1.5 s death pause before restart
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
//  constrainPosition  — slides the ship along walls instead of stopping it.
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
        float dx = c.m_x-p.m_x, dy = c.m_y-p.m_y, dz = c.m_z-p.m_z;
        float d  = dx*dx + dy*dy + dz*dz;
        if (d < best) { best = d; res = c; }
    }
    return res;
}

// ============================================================================
//  isInBounds  — returns false when pos is outside every tunnel zone.
//  Used by projectiles to detect wall impacts.
// ============================================================================
bool DescentGame::isInBounds(const Vector3D& p, float r) const
{
    for (const auto& z : m_zones)
        if (z.containsSphere(p, r)) return true;
    return false;
}

// ============================================================================
//  checkCollisions  — sphere-sphere hit detection, called every frame.
//
//  Uses getEntitiesOfType<T>() each frame so there are never stale pointers
//  after an entity is released mid-frame.
// ============================================================================
void DescentGame::checkCollisions()
{
    auto asteroids      = getEntitiesOfType<Asteroid>();
    auto enemies        = getEntitiesOfType<Enemy>();
    auto projectiles    = getEntitiesOfType<Projectile>();
    auto missiles       = getEntitiesOfType<Missile>();
    auto enemyBolts     = getEntitiesOfType<EnemyProjectile>();

    // --- Player projectiles and missiles vs asteroids ---
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
        // Ramming — hurts the ship, not the asteroid
        if (m_ship)
        {
            float rs = ar + Ship::kShipRadius;
            if (distSq(m_ship->getPosition(), ap) < rs*rs)
                m_ship->TakeDamage(10.f);
        }
        na:;
    }

    // --- Player projectiles and missiles vs enemies ---
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
//
//  Layout (top-down, +Z=south, +X=east):
//
//           [N_ROOM]
//              |
//           [N_ARM]
//              |
//  [W_ROOM]-[W_ARM]-[CENTER]-[E_ARM]-[E_ROOM]
//                      |
//                   [S_ARM]
//                      |
//                   [S_ROOM]  <- player spawns here
//
//  East arm end: upward shaft to [UPPER_E_ROOM]
//  West arm end: downward shaft to [LOWER_W_ROOM]
//
//  Enemies: placed in every room and arm so the player encounters them
//  as they explore.  Enemies do NOT spawn in the player start room so
//  the player has time to get their bearings.
// ============================================================================
void DescentGame::buildMineLevel()
{
    // Lights
    { auto l=createEntity<LightEntity>(); l->setColor({0.9f,0.85f,0.75f}); l->setRotation({-0.4f, 0.6f,0}); }
    { auto l=createEntity<LightEntity>(); l->setColor({0.5f,0.05f,0.05f}); l->setRotation({ 0.6f,-0.6f,0}); }

    // Dimensions (all corridors share the same W×H cross-section)
    const float W  = 200.f;   // corridor half-width  → total 400 u
    const float H  = 130.f;   // corridor half-height → total 260 u
    const float L  = 2000.f;  // arm half-length from center
    const float SD = 400.f;   // extra room depth past arm end
    const float SX = 1600.f;  // X where east shaft begins
    const float SY = 700.f;   // shaft top Y
    const float RY = 450.f;   // upper east room floor Y
    const float RX = 2400.f;  // upper east room east wall X
    const float T  = 25.f;    // wall panel thickness
    const wchar_t* WT = L"../Assets/Textures/wall.jpg";
    const wchar_t* FT = L"../Assets/Textures/brick.png";

    // ---- COLLISION ZONES ------------------------------------------------
    // Each open face extends the stored zone boundary by kShipRadius (15 u)
    // so adjacent zones always overlap — no dead-gap at transitions.

    // Center junction: floor+ceil only, all 4 lateral faces open
    addZone({-W,-H,-W},{W,H,W}, F_MINX|F_MAXX|F_MINZ|F_MAXZ);

    // N–S main corridor
    addZone({-W,-H, W},{W,H, L},  F_MINZ|F_MAXZ);  // south arm
    addZone({-W,-H, L},{W,H,L+SD},F_MINZ);          // south start room
    addZone({-W,-H,-L},{W,H,-W},  F_MINZ|F_MAXZ);  // north arm
    addZone({-W,-H,-L-SD},{W,H,-L},F_MAXZ);         // north room

    // E–W cross corridor
    addZone({-L,-H,-W},{-W,H,W},  F_MINX|F_MAXX);  // west arm
    addZone({ W,-H,-W},{SX,H, W}, F_MINX|F_MAXX);  // east arm west of shaft
    addZone({SX,-H,-W},{ L,H, W}, F_MINX|F_MAXX|F_CEIL);  // east arm over shaft
    addZone({SX, H,-W},{ L,SY,W}, F_MINX|F_MAXX|F_FLOOR); // east shaft vertical
    addZone({ L,RY,-W},{RX,SY,W}, F_MINX);          // upper east room

    // West shaft going DOWN
    addZone({-L,-H,-W},{-SX,H, W}, F_MINX|F_MAXX|F_FLOOR); // west arm under shaft
    addZone({-L,-SY,-W},{-SX,-H,W},F_MINX|F_MAXX|F_CEIL);  // west shaft vertical
    addZone({-RX,-SY,-W},{-L,-RY,W},F_MAXX);        // lower west room

    // ---- VISUAL WALLS ---------------------------------------------------
    addSegWalls({-W,-H,-W},{W,H,W}, F_MINX|F_MAXX|F_MINZ|F_MAXZ, T,WT,FT);

    addSegWalls({-W,-H, W},{W,H, L},  F_MINZ|F_MAXZ, T,WT,FT);
    addSegWalls({-W,-H, L},{W,H,L+SD},F_MINZ,        T,WT,FT);
    addSegWalls({-W,-H,-L},{W,H,-W},  F_MINZ|F_MAXZ, T,WT,FT);
    addSegWalls({-W,-H,-L-SD},{W,H,-L},F_MAXZ,       T,WT,FT);

    addSegWalls({-L,-H,-W},{-W,H,W},  F_MINX|F_MAXX, T,WT,FT);
    addSegWalls({ W,-H,-W},{SX,H, W}, F_MINX|F_MAXX, T,WT,FT);

    // East arm + shaft: hand-built to avoid duplicate panels at SX
    {
        float ex = L, fy = -H, cy = H;
        addPanel({(SX+ex)/2,    fy-T/2, 0}, {ex-SX+2*T, T, 2*W+2*T}, FT); // floor
        addPanel({(SX+ex)/2,   SY+T/2, 0}, {ex-SX+2*T, T, 2*W+2*T}, FT); // shaft top ceil
        addPanel({(SX+ex)/2,    cy+T/2, 0}, {ex-SX+2*T, T, 2*W+2*T}, FT); // arm ceil patch
        addPanel({(SX+ex)/2, (fy+SY)/2,-W-T/2}, {ex-SX+2*T,SY-fy, T}, WT); // front wall
        addPanel({(SX+ex)/2, (fy+SY)/2, W+T/2}, {ex-SX+2*T,SY-fy, T}, WT); // back wall
        addPanel({SX-T/2,  (cy+SY)/2, 0}, {T, SY-cy, 2*W+2*T}, WT);        // shaft west wall
        addPanel({ex+T/2,  (fy+RY)/2, 0}, {T, RY-fy, 2*W+2*T}, WT);        // east wall below room
    }
    addSegWalls({L,RY,-W},{RX,SY,W}, F_MINX, T,WT,FT);

    // West shaft + lower room
    {
        float wx=-L, wsx=-SX, fy=-H, wsY=-SY, rY=-RY;
        addPanel({(wx+wsx)/2,   H+T/2, 0}, {wsx-wx+2*T, T, 2*W+2*T}, FT); // arm ceil
        addPanel({(wx+wsx)/2, wsY-T/2, 0}, {wsx-wx+2*T, T, 2*W+2*T}, FT); // shaft floor
        addPanel({(wx+wsx)/2,   fy-T/2,0}, {wsx-wx+2*T, T, 2*W+2*T}, FT); // arm floor patch
        addPanel({(wx+wsx)/2,(wsY+H)/2,-W-T/2},{wsx-wx+2*T, H-wsY,T}, WT); // front
        addPanel({(wx+wsx)/2,(wsY+H)/2, W+T/2},{wsx-wx+2*T, H-wsY,T}, WT); // back
        addPanel({wsx+T/2,(wsY+fy)/2,0},{T,fy-wsY,2*W+2*T}, WT);           // shaft east wall
        addPanel({wx -T/2,(wsY+rY)/2,0},{T,rY-wsY,2*W+2*T}, WT);           // west wall
    }
    addSegWalls({-RX,-SY,-W},{-L,-RY,W}, F_MAXX, T,WT,FT);

    // ---- ASTEROIDS -------------------------------------------------------
    auto cluster = [&](Vector3D c, int n, float sx, float sy, float sz,
                       float sMin, float sMax)
    {
        for (int i=0;i<n;i++)
            spawnAsteroid({c.m_x+randF(-sx,sx),c.m_y+randF(-sy,sy),c.m_z+randF(-sz,sz)},
                          randF(sMin,sMax));
    };

    cluster({0,0, 2300}, 4, 150, 80, 150, 2.f, 5.f);   // south start room (no enemies)
    cluster({0,0,-2300}, 6, 150, 80, 150, 2.f, 6.f);   // north room
    cluster({0,0,  900}, 3, 120, 70, 400, 2.f, 4.f);   // south arm
    cluster({0,0, -900}, 3, 120, 70, 400, 2.f, 4.f);   // north arm
    cluster({ 1200,0,0}, 3, 500, 70, 120, 2.f, 4.f);   // east arm
    cluster({-1200,0,0}, 3, 500, 70, 120, 2.f, 4.f);   // west arm
    cluster({ 2200,500,0},5, 150, 80, 150, 2.f, 5.f);  // upper east room
    cluster({-2200,-500,0},5,150, 80, 150, 2.f, 5.f);  // lower west room

    // ---- ENEMIES ---------------------------------------------------------
    // Placed so the player encounters enemies as they leave the start room.
    // Each room/arm gets 1–2 enemies.  Start room is deliberately clear so
    // the player has time to orient before combat begins.

    spawnEnemy({   0,   0,   500});    // south arm entrance
    spawnEnemy({   0,   0,  -400});    // center junction south
    spawnEnemy({ 800,   0,     0});    // east arm
    spawnEnemy({-800,   0,     0});    // west arm
    spawnEnemy({   0,   0, -2000});    // north arm
    spawnEnemy({   0,   0, -2500});    // north room guard 1
    spawnEnemy({ 100,  50, -2500});    // north room guard 2
    spawnEnemy({ 2100, 550,   0});     // upper east room
    spawnEnemy({-2100,-550,   0});     // lower west room
}

// ============================================================================
//  addZone
// ============================================================================
void DescentGame::addZone(const Vector3D& mn, const Vector3D& mx, int open)
{
    const float R = Ship::kShipRadius;
    TunnelZone z; z.min=mn; z.max=mx;
    if(open&F_MINX)  z.min.m_x-=R;
    if(open&F_MAXX)  z.max.m_x+=R;
    if(open&F_FLOOR) z.min.m_y-=R;
    if(open&F_CEIL)  z.max.m_y+=R;
    if(open&F_MINZ)  z.min.m_z-=R;
    if(open&F_MAXZ)  z.max.m_z+=R;
    m_zones.push_back(z);
}

// ============================================================================
//  addSegWalls
// ============================================================================
void DescentGame::addSegWalls(const Vector3D& mn, const Vector3D& mx,
                               int open, float T,
                               const wchar_t* wt, const wchar_t* ft)
{
    float cx=(mn.m_x+mx.m_x)*.5f, cy=(mn.m_y+mx.m_y)*.5f, cz=(mn.m_z+mx.m_z)*.5f;
    float sx=mx.m_x-mn.m_x, sy=mx.m_y-mn.m_y, sz=mx.m_z-mn.m_z;
    if(!(open&F_FLOOR)) addPanel({cx,mn.m_y-T*.5f,cz},{sx+2*T,T,sz+2*T},ft); // floor
    if(!(open&F_CEIL))  addPanel({cx,mx.m_y+T*.5f,cz},{sx+2*T,T,sz+2*T},ft); // ceil
    if(!(open&F_MINX))  addPanel({mn.m_x-T*.5f,cy,cz},{T,sy,sz+2*T},   wt); // -X
    if(!(open&F_MAXX))  addPanel({mx.m_x+T*.5f,cy,cz},{T,sy,sz+2*T},   wt); // +X
    if(!(open&F_MINZ))  addPanel({cx,cy,mn.m_z-T*.5f},{sx+2*T,sy,T},   wt); // -Z
    if(!(open&F_MAXZ))  addPanel({cx,cy,mx.m_z+T*.5f},{sx+2*T,sy,T},   wt); // +Z
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
    mat->setCullMode(CullMode::Front); // box normals point out; inside face = back face
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
    if (!e) return nullptr;
    e->setPosition(pos);
    return e;
}
