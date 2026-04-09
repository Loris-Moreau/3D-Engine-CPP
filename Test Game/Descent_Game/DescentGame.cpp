#include "DescentGame.h"

#include <cmath>
#include <cstdlib>
#include <ctime>

static float randF(float lo, float hi)
{
    return lo + (rand() % 10000) / 10000.f * (hi - lo);
}

// =========================================================================
//  onCreate
// =========================================================================
void DescentGame::onCreate()
{
    setTitle(L"Descent");
    srand((unsigned int)time(nullptr));

    // Skybox
    {
        TexturePtr tex = createTexture(L"../Assets/Textures/stars_map.jpg");
        MeshPtr mesh = createMesh(L"../Assets/Meshes/sphere.obj");
        MaterialPtr mat = createMaterial(L"../Assets/Shaders/SkyBox.hlsl");
        mat->addTexture(tex);
        mat->setCullMode(CullMode::Front);
        MeshEntity* sky = createEntity<MeshEntity>();
        sky->setMesh(mesh); sky->addMaterial(mat);
        sky->setScale(Vector3D(20000,20000,20000));
    }

    buildMineLevel();

    m_ship = createEntity<Ship>();
    if (m_ship)
    {
        m_ship->setPosition(Vector3D(0, 0, 2300));
        m_ship->setInitialYaw(3.14159265f);  // face -Z toward corridor
    }
    getInputManager()->enablePlayMode(m_input);
}

// =========================================================================
//  onUpdate
// =========================================================================
void DescentGame::onUpdate(float)
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
// =========================================================================
Vector3D DescentGame::constrainPosition(const Vector3D& p, float r) const
{
    for (auto& z : m_zones) if (z.containsSphere(p,r)) return p;
    float best = 1e30f; Vector3D res = p;
    for (auto& z : m_zones)
    {
        auto c = z.clampSphere(p,r);
        float dx=c.m_x-p.m_x, dy=c.m_y-p.m_y, dz=c.m_z-p.m_z;
        float d=dx*dx+dy*dy+dz*dz;
        if(d<best){best=d;res=c;}
    }
    return res;
}

// =========================================================================
//  isInBounds
// =========================================================================
bool DescentGame::isInBounds(const Vector3D& p, float r) const
{
    for (auto& z : m_zones) if (z.containsSphere(p,r)) return true;
    return false;
}

// =========================================================================
//  checkCollisions
// =========================================================================
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
                m_ship->TakeDamage(10.f);
        }
        na:;
    }
}

// =========================================================================
//  buildMineLevel
//
//  Cross-shaped mine on two vertical levels:
//
//      (UPPER_E) ← shaft up from east arm end
//            [E_ARM]─[CENTER]─[W_ARM]
//                        │
//                    [S_ARM]
//                        │
//                  [S_ROOM]  ← player starts here
//                        │
//                    [N_ARM]
//                        │
//                  [N_ROOM]
//      (LOWER_W) ← shaft down from west arm end
//
//  All corridors are 400 wide x 260 tall (uniform, no frame panels needed).
//  To reach UPPER_E: fly east, ascend shaft at east arm end.
//  To reach LOWER_W: fly west, descend shaft at west arm end.
// =========================================================================
void DescentGame::buildMineLevel()
{
    // Lights
    {
        LightEntity* l = createEntity<LightEntity>();
        l->setColor({0.9f,0.85f,0.75f});
        l->setRotation({-0.4f,0.6f,0});
    }
    {
        LightEntity* l = createEntity<LightEntity>();
        l->setColor({0.5f,0.05f,0.05f});
        l->setRotation({0.6f,-0.6f,0});
    }
    
    // ---- Dimensions ----
    const float W  = 200.0f;  // corridor half-width
    const float H  = 130.0f;  // corridor half-height
    const float L  = 2000.0f; // arm length from center to end
    const float SD = 400.0f;  // room extra depth past arm end
    const float SX = 1600.0f; // X where east shaft begins (within east arm)
    const float SY = 700.0f;  // shaft top Y
    const float RY = 450.0f;  // upper room floor Y
    const float RX = 2400.0f; // upper room east wall X
    const float T  = 25.0f;   // wall panel thickness
    
    const wchar_t* WT = L"../Assets/Textures/wall.jpg";
    const wchar_t* FT = L"../Assets/Textures/brick.png";
    
    // ---- COLLISION ZONES ----
    // Bits: 0=minX  1=maxX  2=minY(floor)  3=maxY(ceil)  4=minZ  5=maxZ
    
    addZone({-W,-H,-W}, {W,H,W},     0|1|4|5);  // center junction (all sides open)
    addZone({-W,-H, W}, {W,H, L},    4|5);       // south arm
    addZone({-W,-H, L}, {W,H, L+SD}, 4);         // south room (open north)
    addZone({-W,-H,-L}, {W,H,-W},    4|5);       // north arm
    addZone({-W,-H,-L-SD},{W,H,-L},  5);         // north room (open south)

    // West arm: east end open (maxX), west end connects to shaft
    addZone({-L,-H,-W}, {-W,H,W},    0|1);       // west arm (fully connected)

    // East arm - split at SX so the shaft ceiling can open
    addZone({ W,-H,-W}, {SX,H,W},    0|1);       // east arm west (normal, both X open)
    addZone({SX,-H,-W}, {L,H,W},     0|1|3);     // east arm east (ceiling open for shaft)
    addZone({SX, H,-W}, {L,SY,W},    0|1|2);     // east shaft vertical (floor open)
    addZone({ L,RY,-W}, {RX,SY,W},   0);         // upper east room (west face open)

    // West shaft (going DOWN) - same logic mirrored
    addZone({-L,-H,-W}, {-SX,H,W},   0|1|2);    // west arm east (floor open for shaft)
    addZone({-L,-SY,-W},{-SX,-H,W},  0|1|3);    // west shaft vertical (ceiling open)
    addZone({-RX,-SY,-W},{-L,-RY,W}, 1);         // lower west room (east face open)

    // ---- VISUAL WALLS ----
    // Uses addSegWalls(min, max, openFaces, T, wallTex, floorTex) defined below.

    // Center
    addSegWalls({-W,-H,-W},{W,H,W},   0|1|4|5,   T,WT,FT);

    // South arm & room
    addSegWalls({-W,-H, W},{W,H, L},  4|5,        T,WT,FT);
    addSegWalls({-W,-H, L},{W,H,L+SD},4,           T,WT,FT);

    // North arm & room
    addSegWalls({-W,-H,-L},{W,H,-W},  4|5,        T,WT,FT);
    addSegWalls({-W,-H,-L-SD},{W,H,-L},5,          T,WT,FT);

    // West arm (normal: no shaft)
    addSegWalls({-L,-H,-W},{-W,H,W},  0|1,        T,WT,FT);

    // East arm west part (normal)
    addSegWalls({W,-H,-W},{SX,H,W},   0|1,        T,WT,FT);

    // East arm, east part + shaft: generate piece by piece
    {
        float ex = L; // east wall X of shaft (= arm end = upper room start)
        float fy = -H, cy = H;  // arm floor/ceil Y

        // Floor of arm-east+shaft (full height from arm floor, shaft goes up above)
        addPanel({(SX+ex)/2, fy-T/2, 0}, {ex-SX+2*T, T, 2*W+2*T}, FT);

        // Arm corridor ceiling: only over west part of shaft zone (there's no ceiling where shaft opens)
        // No arm ceiling in shaft area — shaft is open upward

        // Shaft top ceiling at Y=SY
        addPanel({(SX+ex)/2, SY+T/2, 0}, {ex-SX+2*T, T, 2*W+2*T}, FT);

        // Front wall (minZ=-W), full shaft height
        addPanel({(SX+ex)/2, (fy+SY)/2, -W-T/2}, {ex-SX+2*T, SY-fy, T}, WT);

        // Back wall (maxZ=+W), full shaft height
        addPanel({(SX+ex)/2, (fy+SY)/2, W+T/2},  {ex-SX+2*T, SY-fy, T}, WT);

        // Shaft WEST wall: only above arm ceiling (from cy to SY), at X=SX
        addPanel({SX-T/2, (cy+SY)/2, 0}, {T, SY-cy, 2*W+2*T}, WT);

        // East wall below upper room floor: from arm floor to RY, at X=ex
        addPanel({ex+T/2, (fy+RY)/2, 0}, {T, RY-fy, 2*W+2*T}, WT);
    }

    // Upper east room
    addSegWalls({L,RY,-W},{RX,SY,W},  0,          T,WT,FT);

    // West shaft and lower room (mirror of east, going down)
    {
        float wx  = -L;    // west wall X of west shaft (arm end)
        float wsx = -SX;   // shaft east boundary
        float fy  = -H;
        float wsY = -SY;
        float rY  = -RY;

        // Floor of shaft at Y=wsY (shaft bottom/lower room ceiling is below)
        addPanel({(wx+wsx)/2, wsY-T/2, 0}, {wsx-wx+2*T, T, 2*W+2*T}, FT);

        // Arm floor ceiling (arm goes down into shaft here) = normal arm ceiling at Y=H
        // But floor of arm east part also opens DOWN (shaft continues below)
        // → add arm ceiling for west shaft zone
        addPanel({(wx+wsx)/2, H+T/2, 0}, {wsx-wx+2*T, T, 2*W+2*T}, FT);

        // Front/back walls full shaft depth
        addPanel({(wx+wsx)/2, (wsY+H)/2, -W-T/2}, {wsx-wx+2*T, H-wsY, T}, WT);
        addPanel({(wx+wsx)/2, (wsY+H)/2, W+T/2},  {wsx-wx+2*T, H-wsY, T}, WT);

        // Shaft EAST wall: only below arm floor (from wsY to fy=-H), at X=wsx=-SX
        addPanel({wsx+T/2, (wsY+fy)/2, 0}, {T, fy-wsY, 2*W+2*T}, WT);

        // West wall above lower room ceiling: from rY to fy, at X=wx
        addPanel({wx-T/2, (rY+fy)/2, 0}, {T, fy-rY, 2*W+2*T}, WT);
    }

    // Lower west room
    addSegWalls({-RX,-SY,-W},{-L,-RY,W}, 1,       T,WT,FT);

    // ---- ASTEROIDS ----
    auto cluster = [&](Vector3D c, int n, float sx, float sy, float sz, float sMin, float sMax)
    {
        for (int i=0;i<n;i++)
        {
            spawnAsteroid(
                {c.m_x+randF(-sx,sx), c.m_y+randF(-sy,sy), c.m_z+randF(-sz,sz)},
                randF(sMin,sMax));
        }
    };

    cluster({0,0, 2300}, 4, 150, 80, 150, 2.f, 5.f);   // south room
    cluster({0,0,-2300}, 6, 150, 80, 150, 2.f, 6.f);   // north room
    cluster({0,0, 1000}, 3, 120, 70, 400, 1.5f,4.f);   // south arm
    cluster({0,0,-1000}, 3, 120, 70, 400, 1.5f,4.f);   // north arm
    cluster({ 1200,0,0}, 3, 500, 70, 120, 1.5f,4.f);   // east arm
    cluster({-1200,0,0}, 3, 500, 70, 120, 1.5f,4.f);   // west arm
    cluster({ 2200,500,0},5, 150, 80, 150, 2.f,5.f);   // upper east room
    cluster({-2200,-500,0},5,150, 80, 150, 2.f,5.f);   // lower west room
}

// =========================================================================
//  addZone
//  Registers a collision zone, extending each open face by kShipRadius
//  so adjacent zones always overlap (no dead-gap at transitions).
// =========================================================================
void DescentGame::addZone(const Vector3D& mn, const Vector3D& mx, int openFaces)
{
    const float R = Ship::kShipRadius;
    TunnelZone z;
    z.min = mn; z.max = mx;
    if (openFaces & (1<<0)) z.min.m_x -= R;
    if (openFaces & (1<<1)) z.max.m_x += R;
    if (openFaces & (1<<2)) z.min.m_y -= R;
    if (openFaces & (1<<3)) z.max.m_y += R;
    if (openFaces & (1<<4)) z.min.m_z -= R;
    if (openFaces & (1<<5)) z.max.m_z += R;
    m_zones.push_back(z);
}

// =========================================================================
//  addSegWalls — auto-generate 6 box-face panels, skipping open faces.
//  Panel thickness T is added outward of the zone boundary.
// =========================================================================
void DescentGame::addSegWalls(const Vector3D& mn, const Vector3D& mx,
                               int openFaces, float T,
                               const wchar_t* wt, const wchar_t* ft)
{
    float cx=(mn.m_x+mx.m_x)*.5f, cy=(mn.m_y+mx.m_y)*.5f, cz=(mn.m_z+mx.m_z)*.5f;
    float sx=mx.m_x-mn.m_x, sy=mx.m_y-mn.m_y, sz=mx.m_z-mn.m_z;

    if (!(openFaces&(1<<2))) addPanel({cx, mn.m_y-T*.5f, cz}, {sx+2*T, T, sz+2*T}, ft); // floor
    if (!(openFaces&(1<<3))) addPanel({cx, mx.m_y+T*.5f, cz}, {sx+2*T, T, sz+2*T}, ft); // ceil
    if (!(openFaces&(1<<0))) addPanel({mn.m_x-T*.5f, cy, cz}, {T, sy,   sz+2*T},   wt); // -X
    if (!(openFaces&(1<<1))) addPanel({mx.m_x+T*.5f, cy, cz}, {T, sy,   sz+2*T},   wt); // +X
    if (!(openFaces&(1<<4))) addPanel({cx, cy, mn.m_z-T*.5f}, {sx+2*T, sy, T},     wt); // -Z
    if (!(openFaces&(1<<5))) addPanel({cx, cy, mx.m_z+T*.5f}, {sx+2*T, sy, T},     wt); // +Z
}

// =========================================================================
//  addPanel — place one wall box mesh
// =========================================================================
void DescentGame::addPanel(const Vector3D& center, const Vector3D& scale,
                            const wchar_t* texPath)
{
    auto tex  = createTexture(texPath);
    auto mesh = createMesh(L"../Assets/Meshes/box.obj");
    auto mat  = createMaterial(L"../Assets/Shaders/base.hlsl");
    mat->addTexture(tex);
    mat->setCullMode(CullMode::Front); // faces seen from inside = back faces of box

    auto e = createEntity<MeshEntity>();
    e->setMesh(mesh); e->addMaterial(mat);
    e->setPosition(center);
    e->setScale(scale);
}

// =========================================================================
//  spawnAsteroid
// =========================================================================
Asteroid* DescentGame::spawnAsteroid(const Vector3D& pos, float scale)
{
    auto* a = createEntity<Asteroid>();
    if (!a) return nullptr;
    a->setPosition(pos);
    a->setScale({scale,scale,scale});
    a->setCollisionRadius(scale * 5.5f);
    a->setRotation({randF(0,6.28f), randF(0,6.28f), randF(0,6.28f)});
    return a;
}
