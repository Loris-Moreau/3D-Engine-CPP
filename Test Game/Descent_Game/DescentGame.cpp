/*
 * DescentGame.cpp
 *
 * Implementation of the Descent mine level.
 *
 * Key design notes
 * ----------------
 * Wall panel seams:
 *   addSegWalls() extends each panel only at its CLOSED faces (the faces
 *   that have a solid wall cap).  Open-face ends are left flush with the
 *   zone boundary.  This means neighbouring zone panels touch exactly at
 *   the shared boundary — no overlap, no Z-fighting, no visible seam.
 *
 * Zone overlap at transitions:
 *   addZone() extends each open boundary by kShipRadius (15 u) so that
 *   adjacent zones always overlap.  containsSphere() insets by the same
 *   radius, so the ship is continuously accepted by at least one zone
 *   when crossing a boundary — no invisible-wall dead-gap.
 *
 * openFaces bitmask (WARNING: use the named constants, NOT bare integers):
 *   Writing "4|5" evaluates to decimal 5 (bitwise OR of integers 4 and 5),
 *   NOT to bit 4 and bit 5.  Always use F_MINX, F_MAXX, F_FLOOR etc.
 */

#include "DescentGame.h"
#include <ctime>
#include <cstdlib>
#include <cmath>

// ---------------------------------------------------------------------------
// Named face-open constants for the openFaces bitmask.
// These replace the raw integers that caused the original "4|5 == 5" bug.
// ---------------------------------------------------------------------------
static constexpr int F_MINX  = (1 << 0);   // left face  (−X)
static constexpr int F_MAXX  = (1 << 1);   // right face (+X)
static constexpr int F_FLOOR = (1 << 2);   // floor      (−Y)
static constexpr int F_CEIL  = (1 << 3);   // ceiling    (+Y)
static constexpr int F_MINZ  = (1 << 4);   // near face  (−Z)
static constexpr int F_MAXZ  = (1 << 5);   // far face   (+Z)

// Uniform random float in [lo, hi).
static float randF(float lo, float hi)
{
    return lo + (rand() % 10000) / 10000.f * (hi - lo);
}

// ============================================================================
//  onCreate  — runs once at startup
// ============================================================================
void DescentGame::onCreate()
{
    setTitle(L"Descent - Mines of Zeta Aquilae");
    srand((unsigned int)time(nullptr));

    // Skybox: a large inverted sphere with the star-map texture.
    // CullMode::Front makes the inside faces visible.
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

    buildMineLevel();   // fills m_zones and spawns all wall + asteroid entities

    // Spawn the player ship inside the south start room, facing −Z (toward the
    // main corridor).  Must come AFTER buildMineLevel so m_zones is populated.
    m_ship = createEntity<Ship>();
    if (m_ship)
    {
        m_ship->setPosition(Vector3D(0, 0, 2300));
        m_ship->setInitialYaw(3.14159265f);
    }

    getInputManager()->enablePlayMode(m_input);
}

// ============================================================================
//  onUpdate  — runs every frame
// ============================================================================
void DescentGame::onUpdate(float)
{
    // Escape toggles cursor lock (play mode ↔ windowed mouse).
    if (getInputManager()->isKeyUp(Key::Escape))
    {
        m_input = !m_input;
        getInputManager()->enablePlayMode(m_input);
    }

    checkCollisions();
}

// ============================================================================
//  constrainPosition  (override of Game::constrainPosition)
//
//  Called by Ship::onUpdate with the desired new position.
//  Returns the position unchanged if it is already inside any zone.
//  Otherwise finds the zone whose boundary is nearest and projects the
//  position onto that boundary so the ship slides along walls.
// ============================================================================
Vector3D DescentGame::constrainPosition(const Vector3D& p, float r) const
{
    for (const auto& z : m_zones)
        if (z.containsSphere(p, r)) return p;

    float    best = 1e30f;
    Vector3D res  = p;
    for (const auto& z : m_zones)
    {
        Vector3D c  = z.clampSphere(p, r);
        float dx = c.m_x - p.m_x, dy = c.m_y - p.m_y, dz = c.m_z - p.m_z;
        float d  = dx*dx + dy*dy + dz*dz;
        if (d < best) { best = d; res = c; }
    }
    return res;
}

// ============================================================================
//  isInBounds  (override of Game::isInBounds)
//
//  Called by Projectile::onUpdate and Missile::onUpdate each frame.
//  Returns false the moment a projectile would move outside every zone,
//  causing it to be destroyed (simulating a wall impact).
// ============================================================================
bool DescentGame::isInBounds(const Vector3D& p, float r) const
{
    for (const auto& z : m_zones)
        if (z.containsSphere(p, r)) return true;
    return false;
}

// ============================================================================
//  checkCollisions  — sphere-sphere hit detection, called every frame
//
//  Uses getEntitiesOfType<T>() each frame rather than a cached pointer list.
//  This guarantees stale pointers are never accessed after an entity is
//  released mid-frame (e.g. an asteroid destroyed on the same frame it is
//  rammed by the ship).
//
//  Damage rules:
//    Laser (Projectile, left-click)   : 25 hp  — 3 hits to destroy an asteroid
//    Missile (Missile, right-click)   : 75 hp  — 1 hit  to destroy an asteroid
//    Ramming (ship touching asteroid) : 10 hp to the ship, asteroid unharmed
// ============================================================================
void DescentGame::checkCollisions()
{
    auto asteroids   = getEntitiesOfType<Asteroid>();
    auto projectiles = getEntitiesOfType<Projectile>();
    auto missiles    = getEntitiesOfType<Missile>();

    for (Asteroid* a : asteroids)
    {
        if (!a || a->m_health <= 0.f) continue;

        float ar = a->getCollisionRadius();
        auto  ap = a->getPosition();

        // Laser hits
        for (Projectile* p : projectiles)
        {
            float rs = ar + p->getCollisionRadius();
            if (distSq(p->getPosition(), ap) < rs * rs)
            {
                a->TakeDamage(p->GetDamage());
                p->release();
                if (a->m_health <= 0.f) goto next_asteroid;
            }
        }

        // Missile hits
        for (Missile* m : missiles)
        {
            float rs = ar + m->getCollisionRadius();
            if (distSq(m->getPosition(), ap) < rs * rs)
            {
                a->TakeDamage(m->GetDamage());
                m->release();
                if (a->m_health <= 0.f) goto next_asteroid;
            }
        }

        // Ship ramming — hurts the ship, not the asteroid
        if (m_ship)
        {
            float rs = ar + Ship::kShipRadius;
            if (distSq(m_ship->getPosition(), ap) < rs * rs)
                m_ship->TakeDamage(10.f);
        }

        next_asteroid:;
    }
}

// ============================================================================
//  buildMineLevel
// ============================================================================
void DescentGame::buildMineLevel()
{
    // Two directional lights: a warm white main light and a dim red fill.
    { auto l = createEntity<LightEntity>(); l->setColor({0.9f, 0.85f, 0.75f}); l->setRotation({-0.4f,  0.6f, 0}); }
    { auto l = createEntity<LightEntity>(); l->setColor({0.5f, 0.05f, 0.05f}); l->setRotation({ 0.6f, -0.6f, 0}); }

    // ---- Shared dimensions -----------------------------------------------
    // All corridors and rooms use the same W/H cross-section so panels join
    // flush at every junction — no need for separate frame panels.
    const float W  = 200.f;   // corridor half-width  → total 400 u
    const float H  = 130.f;   // corridor half-height → total 260 u
    const float L  = 2000.f;  // arm half-length from the center junction
    const float RD = 400.f;   // extra depth of each terminal room past the arm end
    const float SX = 1600.f;  // X coordinate where the east shaft begins (within the east arm)
    const float SY = 700.f;   // top Y of the east shaft / bottom Y of the upper east room
    const float RY = 450.f;   // floor Y of the upper east room
    const float RX = 2400.f;  // east wall X of the upper east room
    const float T  = 25.f;    // wall panel thickness

    const wchar_t* WT = L"../Assets/Textures/wall.jpg";    // side walls
    const wchar_t* FT = L"../Assets/Textures/brick.png";   // floors and ceilings

    // =========================================================================
    // COLLISION ZONES
    //
    // Each addZone() call registers a TunnelZone.  Open faces (marked with
    // F_* flags) are extended by kShipRadius so adjacent zones overlap —
    // this eliminates the dead-gap that would otherwise occur at transitions.
    // =========================================================================

    // Center cross junction — floor and ceiling only; all four lateral faces open.
    addZone({-W,-H,-W}, {W,H,W},   F_MINX|F_MAXX|F_MINZ|F_MAXZ);

    // N–S main corridor
    addZone({-W,-H,  W}, {W,H,  L},  F_MINZ|F_MAXZ);   // south arm
    addZone({-W,-H,  L}, {W,H,L+RD}, F_MINZ);           // south start room (open north end)
    addZone({-W,-H, -L}, {W,H, -W},  F_MINZ|F_MAXZ);   // north arm
    addZone({-W,-H,-L-RD},{W,H,  -L},F_MAXZ);           // north dead-end room (open south end)

    // E–W cross corridor
    addZone({-L,-H,-W},{-W, H, W},  F_MINX|F_MAXX);   // west arm (normal, no shaft)
    addZone({ W,-H,-W},{SX, H, W},  F_MINX|F_MAXX);   // east arm — west of shaft (normal)
    addZone({SX,-H,-W},{ L, H, W},  F_MINX|F_MAXX|F_CEIL);  // east arm — over shaft (ceiling open upward)
    addZone({SX, H,-W},{ L,SY, W},  F_MINX|F_MAXX|F_FLOOR); // east shaft vertical (floor open → arm below)
    addZone({ L,RY,-W},{RX,SY, W},  F_MINX);           // upper east room (west face open → shaft)

    // West shaft going DOWN (mirror of the east shaft)
    addZone({-L,-H,-W},{-SX, H, W}, F_MINX|F_MAXX|F_FLOOR); // west arm — under shaft (floor open downward)
    addZone({-L,-SY,-W},{-SX,-H,W}, F_MINX|F_MAXX|F_CEIL);  // west shaft vertical (ceiling open → arm above)
    addZone({-RX,-SY,-W},{-L,-RY,W},F_MAXX);           // lower west room (east face open → shaft)

    // =========================================================================
    // VISUAL WALLS
    //
    // addSegWalls() uses the same zone bounds and openFaces as addZone() above.
    // Panels extend by T only at CLOSED faces, keeping shared boundaries flush.
    // =========================================================================

    // Center junction
    addSegWalls({-W,-H,-W},{W,H,W}, F_MINX|F_MAXX|F_MINZ|F_MAXZ, T,WT,FT);

    // N–S south
    addSegWalls({-W,-H,  W},{W,H,  L},  F_MINZ|F_MAXZ, T,WT,FT);
    addSegWalls({-W,-H,  L},{W,H,L+RD}, F_MINZ,        T,WT,FT);

    // N–S north
    addSegWalls({-W,-H, -L},{W,H,  -W},  F_MINZ|F_MAXZ, T,WT,FT);
    addSegWalls({-W,-H,-L-RD},{W,H,-L},  F_MAXZ,        T,WT,FT);

    // E–W west (no shaft — simple box)
    addSegWalls({-L,-H,-W},{-W,H,W}, F_MINX|F_MAXX, T,WT,FT);

    // E–W east — west-of-shaft segment (normal)
    addSegWalls({W,-H,-W},{SX,H,W}, F_MINX|F_MAXX, T,WT,FT);

    // East arm + shaft: generated panel-by-panel because the ceiling is partly
    // open (the shaft rises through it) and the zone is split at X = SX.
    {
        // Arm floor runs the full X extent from SX to L (no Y complications here)
        addPanel({(SX+L)/2,       -H-T/2, 0}, {L-SX+2*T,    T,  2*W+2*T}, FT);
        // Shaft ceiling at Y = SY (top of the shaft / floor of the upper room)
        addPanel({(SX+L)/2,      SY+T/2,  0}, {L-SX+2*T,    T,  2*W+2*T}, FT);
        // Arm ceiling patch between SX and L (solid ceiling of the arm section,
        // placed at the same Y as the normal arm ceiling = H)
        addPanel({(SX+L)/2,       H+T/2,  0}, {L-SX+2*T,    T,  2*W+2*T}, FT);
        // Front wall (−Z side) spanning full shaft height from arm floor to shaft top
        addPanel({(SX+L)/2, (-H+SY)/2,   -W-T/2}, {L-SX+2*T, SY+H, T},   WT);
        // Back wall  (+Z side) spanning full shaft height
        addPanel({(SX+L)/2, (-H+SY)/2,    W+T/2}, {L-SX+2*T, SY+H, T},   WT);
        // Shaft west wall: only above the arm ceiling (from H to SY), at X = SX
        addPanel({SX-T/2,   (H+SY)/2,         0}, {T,  SY-H,  2*W+2*T},   WT);
        // East wall: below the upper room floor (from −H to RY), at X = L
        addPanel({L+T/2,    (-H+RY)/2,        0}, {T,  RY+H,  2*W+2*T},   WT);
    }
    addSegWalls({L,RY,-W},{RX,SY,W}, F_MINX, T,WT,FT);

    // West shaft + lower room (mirror geometry, shaft goes DOWN)
    {
        const float wx  = -L;    // west boundary of the shaft section
        const float wsx = -SX;   // east boundary of the shaft section
        const float wsY = -SY;   // shaft bottom Y (= lower room ceiling Y)
        const float rY  = -RY;   // lower room ceiling Y

        // Arm ceiling (at Y = H) over the shaft section
        addPanel({(wx+wsx)/2,  H+T/2, 0}, {wsx-wx+2*T, T,  2*W+2*T}, FT);
        // Shaft floor at Y = wsY (= −SY)
        addPanel({(wx+wsx)/2, wsY-T/2, 0},{wsx-wx+2*T, T,  2*W+2*T}, FT);
        // Arm floor patch between wx and wsx
        addPanel({(wx+wsx)/2, -H-T/2,  0},{wsx-wx+2*T, T,  2*W+2*T}, FT);
        // Front wall (−Z) spanning full shaft height
        addPanel({(wx+wsx)/2, (-H+wsY)/2, -W-T/2}, {wsx-wx+2*T, -H-wsY, T}, WT);
        // Back wall  (+Z) spanning full shaft height
        addPanel({(wx+wsx)/2, (-H+wsY)/2,  W+T/2}, {wsx-wx+2*T, -H-wsY, T}, WT);
        // Shaft east wall: only below the arm floor (from wsY to −H), at X = wsx
        addPanel({wsx+T/2,  (wsY-H)/2,   0}, {T,   -H-wsY,  2*W+2*T}, WT);
        // West wall: above the lower room ceiling (from wsY to rY), at X = wx
        addPanel({wx -T/2,  (wsY+rY)/2,  0}, {T,   rY-wsY,  2*W+2*T}, WT);
    }
    addSegWalls({-RX,-SY,-W},{-L,-RY,W}, F_MAXX, T,WT,FT);

    // =========================================================================
    // ASTEROIDS
    //
    // cluster(centre, count, spreadX, spreadY, spreadZ, scaleMin, scaleMax)
    // Each asteroid is placed at a random offset from the cluster centre.
    // scale * 5.5 = collision radius (empirically chosen to match asteroid.obj).
    // =========================================================================
    auto cluster = [&](Vector3D c, int n, float sx, float sy, float sz,
                       float sMin, float sMax)
    {
        for (int i = 0; i < n; i++)
            spawnAsteroid(
                { c.m_x + randF(-sx,sx), c.m_y + randF(-sy,sy), c.m_z + randF(-sz,sz) },
                randF(sMin, sMax));
    };

    cluster({  0,   0,  2200}, 4, 140, 80, 140, 2.f, 5.f);  // south start room
    cluster({  0,   0, -2300}, 6, 150, 80, 150, 2.f, 6.f);  // north room
    cluster({  0,   0,   900}, 3, 120, 70, 500, 2.f, 4.f);  // south arm
    cluster({  0,   0,  -900}, 3, 120, 70, 500, 2.f, 4.f);  // north arm
    cluster({ 1200, 0,     0}, 3, 500, 70, 120, 2.f, 4.f);  // east arm
    cluster({-1200, 0,     0}, 3, 500, 70, 120, 2.f, 4.f);  // west arm
    cluster({ 2200, 550,   0}, 5, 140, 60, 140, 2.f, 5.f);  // upper east room
    cluster({-2200,-550,   0}, 5, 140, 60, 140, 2.f, 5.f);  // lower west room
}

// ============================================================================
//  addZone
//
//  Registers a collision zone.  Open faces are extended outward by
//  kShipRadius (15 u) so that adjacent zones always overlap at their shared
//  boundary.  containsSphere() insets by the same radius, meaning the ship
//  is continuously accepted by at least one zone when crossing the boundary.
// ============================================================================
void DescentGame::addZone(const Vector3D& mn, const Vector3D& mx, int open)
{
    const float R = Ship::kShipRadius;
    TunnelZone z;
    z.min = mn;  z.max = mx;
    if (open & F_MINX)  z.min.m_x -= R;
    if (open & F_MAXX)  z.max.m_x += R;
    if (open & F_FLOOR) z.min.m_y -= R;
    if (open & F_CEIL)  z.max.m_y += R;
    if (open & F_MINZ)  z.min.m_z -= R;
    if (open & F_MAXZ)  z.max.m_z += R;
    m_zones.push_back(z);
}

// ============================================================================
//  addSegWalls
//
//  Generates up to 6 wall panels (one per face) for the box [mn, mx].
//  Faces listed in openFaces are skipped entirely.
//
//  Panel extension rule — the critical fix for texture seams:
//    Each panel is extended by thickness T only at CLOSED faces.
//    At open faces the panel stops flush with the zone boundary.
//    This ensures adjacent zone panels meet exactly — no overlap, no gap,
//    no Z-fighting seam at corridor junctions.
//
//  Example — a closed-end wall cap:
//    Near wall (F_MINZ closed):  floor/ceiling extend to mn.z − T so they
//    cover the corner region that the near wall cap occupies.
//
//  Example — an open corridor-to-corridor transition (F_MINZ open):
//    Floor stops at mn.z exactly.  Adjacent zone's floor also starts at
//    mn.z exactly.  No overlap.
// ============================================================================
void DescentGame::addSegWalls(const Vector3D& mn, const Vector3D& mx,
                               int open, float T,
                               const wchar_t* wt, const wchar_t* ft)
{
    // Compute the effective extents for each axis.
    // Open ends stop at the zone boundary; closed ends extend by T.
    const float x0 = (open & F_MINX)  ? mn.m_x : mn.m_x - T;
    const float x1 = (open & F_MAXX)  ? mx.m_x : mx.m_x + T;
    const float y0 = (open & F_FLOOR) ? mn.m_y : mn.m_y - T;
    const float y1 = (open & F_CEIL)  ? mx.m_y : mx.m_y + T;
    const float z0 = (open & F_MINZ)  ? mn.m_z : mn.m_z - T;
    const float z1 = (open & F_MAXZ)  ? mx.m_z : mx.m_z + T;

    // Panel sizes and centres derived from those extents
    const float fsx = x1 - x0,  fcx = (x0 + x1) * .5f;   // floor/ceil panel: X size and centre
    const float fsz = z1 - z0,  fcz = (z0 + z1) * .5f;   // floor/ceil panel: Z size and centre
    const float wsy = y1 - y0,  wcy = (y0 + y1) * .5f;   // wall panel:       Y size and centre

    if (!(open & F_FLOOR)) addPanel({fcx, mn.m_y - T*.5f, fcz}, {fsx, T,  fsz}, ft);  // floor
    if (!(open & F_CEIL))  addPanel({fcx, mx.m_y + T*.5f, fcz}, {fsx, T,  fsz}, ft);  // ceiling
    if (!(open & F_MINX))  addPanel({mn.m_x - T*.5f, wcy, fcz}, {T,  wsy, fsz}, wt);  // left  (−X)
    if (!(open & F_MAXX))  addPanel({mx.m_x + T*.5f, wcy, fcz}, {T,  wsy, fsz}, wt);  // right (+X)
    if (!(open & F_MINZ))  addPanel({fcx, wcy, mn.m_z - T*.5f}, {fsx, wsy, T},  wt);  // near  (−Z)
    if (!(open & F_MAXZ))  addPanel({fcx, wcy, mx.m_z + T*.5f}, {fsx, wsy, T},  wt);  // far   (+Z)
}

// ============================================================================
//  addPanel
//
//  Places one scaled box-mesh wall panel at world position centre.
//  CullMode::Front is required because box.obj normals point outward — from
//  inside the corridor the visible surface is the box's back face.
// ============================================================================
void DescentGame::addPanel(const Vector3D& centre, const Vector3D& scale,
                            const wchar_t* texPath)
{
    auto tex  = createTexture(texPath);
    auto mesh = createMesh(L"../Assets/Meshes/box.obj");
    auto mat  = createMaterial(L"../Assets/Shaders/base.hlsl");
    mat->addTexture(tex);
    mat->setCullMode(CullMode::Front);

    auto e = createEntity<MeshEntity>();
    e->setMesh(mesh);
    e->addMaterial(mat);
    e->setPosition(centre);
    e->setScale(scale);
}

// ============================================================================
//  spawnAsteroid
// ============================================================================
Asteroid* DescentGame::spawnAsteroid(const Vector3D& pos, float scale)
{
    auto* a = createEntity<Asteroid>();
    if (!a) return nullptr;
    a->setPosition(pos);
    a->setScale({scale, scale, scale});
    a->setCollisionRadius(scale * 5.5f);   // must be called after setScale
    a->setRotation({randF(0, 6.28f), randF(0, 6.28f), randF(0, 6.28f)});
    return a;
}
