#include "DescentGame.h"
#include <ctime>
#include <cstdlib>
#include <cmath>

static float randF(float lo, float hi)
{
    return lo + (rand() % 10000) / 10000.f * (hi - lo);
}

// Named face-open constants — avoids the decimal-literal trap:
// 4|5 == 5 (minX|floor), NOT bit4|bit5 (minZ|maxZ).
static constexpr int F_MINX  = (1<<0);
static constexpr int F_MAXX  = (1<<1);
static constexpr int F_FLOOR = (1<<2);
static constexpr int F_CEIL  = (1<<3);
static constexpr int F_MINZ  = (1<<4);
static constexpr int F_MAXZ  = (1<<5);

void DescentGame::onCreate()
{
    setTitle(L"Descent - Mines of Zeta Aquilae");
    srand((unsigned int)time(nullptr));

    {
        auto tex  = createTexture(L"../Assets/Textures/stars_map.jpg");
        auto mesh = createMesh(L"../Assets/Meshes/sphere.obj");
        auto mat  = createMaterial(L"../Assets/Shaders/SkyBox.hlsl");
        mat->addTexture(tex);
        mat->setCullMode(CullMode::Front);
        auto sky = createEntity<MeshEntity>();
        sky->setMesh(mesh); sky->addMaterial(mat);
        sky->setScale(Vector3D(20000,20000,20000));
    }

    buildMineLevel();

    m_ship = createEntity<Ship>();
    if (m_ship)
    {
        m_ship->setPosition(Vector3D(0, 0, 2300));
        m_ship->setInitialYaw(3.14159265f);
    }
    getInputManager()->enablePlayMode(m_input);
}

void DescentGame::onUpdate(float)
{
    if (getInputManager()->isKeyUp(Key::Escape))
    {
        m_input = !m_input;
        getInputManager()->enablePlayMode(m_input);
    }
    checkCollisions();
}

Vector3D DescentGame::constrainPosition(const Vector3D& p, float r) const
{
    for (auto& z : m_zones) if (z.containsSphere(p,r)) return p;
    float best=1e30f; Vector3D res=p;
    for (auto& z : m_zones)
    {
        auto c=z.clampSphere(p,r);
        float dx=c.m_x-p.m_x, dy=c.m_y-p.m_y, dz=c.m_z-p.m_z;
        float d=dx*dx+dy*dy+dz*dz;
        if(d<best){best=d;res=c;}
    }
    return res;
}

bool DescentGame::isInBounds(const Vector3D& p, float r) const
{
    for (auto& z : m_zones) if (z.containsSphere(p,r)) return true;
    return false;
}

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
//  Layout (top-down, +Z = south, +X = east):
//
//             [N_ROOM]
//                |
//             [N_ARM]   Z: -2000 to -400
//                |
//   [W_ROOM]-[W_ARM]-[CENTER]-[E_ARM]-[E_ROOM]
//                |
//             [S_ARM]   Z: +400 to +2000
//                |
//             [S_ROOM]  <-- player spawns here (Z=2300)
//
//  Vertical element: east arm end has upward shaft to [UPPER_E_ROOM]
//                    west arm end has downward shaft to [LOWER_W_ROOM]
//
//  ALL corridors and rooms use the same cross-section (W=200 half-width,
//  H=130 half-height = 400u wide x 260u tall) so wall panels always
//  connect flush — no frame panels needed at junctions.
// =========================================================================
void DescentGame::buildMineLevel()
{
    { auto l=createEntity<LightEntity>(); l->setColor({0.9f,0.85f,0.75f}); l->setRotation({-0.4f,0.6f,0}); }
    { auto l=createEntity<LightEntity>(); l->setColor({0.5f,0.05f,0.05f}); l->setRotation({0.6f,-0.6f,0}); }

    const float W  = 200.f;   // corridor half-width  (total 400)
    const float H  = 130.f;   // corridor half-height (total 260)
    const float L  = 2000.f;  // arm half-length from center
    const float RD = 400.f;   // room extra depth past arm end
    const float SX = 1600.f;  // X where east shaft begins (within east arm)
    const float SY = 700.f;   // shaft top Y
    const float RY = 450.f;   // upper room floor Y
    const float RX = 2400.f;  // upper room east wall X
    const float T  = 25.f;    // wall panel thickness
    const wchar_t* WT = L"../Assets/Textures/wall.jpg";
    const wchar_t* FT = L"../Assets/Textures/brick.png";

    // ---------------------------------------------------------------
    // COLLISION ZONES
    // addZone extends each open face by kShipRadius=15 so adjacent
    // zones always overlap — no dead-gap at transitions.
    // ---------------------------------------------------------------

    // Center junction: floor+ceil only, all 4 side faces open
    addZone({-W,-H,-W}, {W,H,W},   F_MINX|F_MAXX|F_MINZ|F_MAXZ);

    // N-S corridors
    addZone({-W,-H, W}, {W,H, L},  F_MINZ|F_MAXZ);   // south arm
    addZone({-W,-H, L}, {W,H,L+RD},F_MINZ);           // south start room
    addZone({-W,-H,-L}, {W,H,-W},  F_MINZ|F_MAXZ);   // north arm
    addZone({-W,-H,-L-RD},{W,H,-L},F_MAXZ);           // north room

    // E-W corridors (full arms — shaft segments added separately below)
    addZone({-L,-H,-W},{-W,H,W},   F_MINX|F_MAXX);   // west arm
    addZone({ W,-H,-W},{SX,H, W},  F_MINX|F_MAXX);   // east arm (west of shaft)
    addZone({SX,-H,-W},{L, H, W},  F_MINX|F_MAXX|F_CEIL); // east arm (over shaft, ceil open)
    addZone({SX, H,-W},{L, SY,W},  F_MINX|F_MAXX|F_FLOOR);// east shaft (floor open → arm)
    addZone({ L,RY,-W},{RX,SY,W},  F_MINX);           // upper east room

    // West shaft going DOWN
    addZone({-L,-H,-W},{-SX,H, W}, F_MINX|F_MAXX|F_FLOOR);// west arm (under shaft, floor open)
    addZone({-L,-SY,-W},{-SX,-H,W},F_MINX|F_MAXX|F_CEIL); // west shaft (ceil open → arm)
    addZone({-RX,-SY,-W},{-L,-RY,W},F_MAXX);          // lower west room

    // ---------------------------------------------------------------
    // VISUAL WALLS — identical bounds to zones above, same openFaces
    // ---------------------------------------------------------------
    addSegWalls({-W,-H,-W},{W,H,W}, F_MINX|F_MAXX|F_MINZ|F_MAXZ, T,WT,FT);

    addSegWalls({-W,-H, W},{W,H, L},  F_MINZ|F_MAXZ, T,WT,FT);
    addSegWalls({-W,-H, L},{W,H,L+RD},F_MINZ,        T,WT,FT);
    addSegWalls({-W,-H,-L},{W,H,-W},  F_MINZ|F_MAXZ, T,WT,FT);
    addSegWalls({-W,-H,-L-RD},{W,H,-L},F_MAXZ,       T,WT,FT);

    addSegWalls({-L,-H,-W},{-W,H,W},  F_MINX|F_MAXX, T,WT,FT);
    addSegWalls({ W,-H,-W},{SX,H, W}, F_MINX|F_MAXX, T,WT,FT);

    // East arm + shaft: hand-build to avoid duplicate/missing panels at SX boundary
    {
        // Floor from SX to L (arm + shaft base)
        addPanel({(SX+L)/2,  -H-T/2, 0}, {L-SX+2*T, T, 2*W+2*T}, FT);
        // Shaft top ceiling at SY
        addPanel({(SX+L)/2, SY+T/2,  0}, {L-SX+2*T, T, 2*W+2*T}, FT);
        // Arm ceiling from SX east (only up to SX — west of SX has normal ceiling from arm-west)
        addPanel({(SX+L)/2,  H+T/2,  0}, {L-SX+2*T, T, 2*W+2*T}, FT);
        // Mask: a wall patch blocks the arm ceiling where the shaft opens
        // (no extra needed — shaft is above arm ceiling, addSegWalls for arm-west stops at SX)

        // Front wall (minZ=-W) full shaft height
        addPanel({(SX+L)/2, (-H+SY)/2, -W-T/2}, {L-SX+2*T, SY-(-H), T}, WT);
        // Back wall (maxZ=+W) full shaft height
        addPanel({(SX+L)/2, (-H+SY)/2,  W+T/2}, {L-SX+2*T, SY-(-H), T}, WT);
        // Shaft west wall: above arm ceiling (H to SY), at X=SX
        addPanel({SX-T/2, (H+SY)/2, 0}, {T, SY-H, 2*W+2*T}, WT);
        // East wall below upper room floor (−H to RY), at X=L
        addPanel({L+T/2,  (-H+RY)/2, 0}, {T, RY-(-H), 2*W+2*T}, WT);
    }
    addSegWalls({L,RY,-W},{RX,SY,W}, F_MINX, T,WT,FT);

    // West shaft (mirror east, going DOWN)
    {
        float wx=-L, wsx=-SX, wsY=-SY, rY=-RY;
        // Ceiling from wsx to wx (arm + shaft top)
        addPanel({(wx+wsx)/2,  H+T/2, 0}, {wsx-wx+2*T, T, 2*W+2*T}, FT);
        // Shaft bottom floor at wsY
        addPanel({(wx+wsx)/2, wsY-T/2, 0},{wsx-wx+2*T, T, 2*W+2*T}, FT);
        // Arm floor under shaft section
        addPanel({(wx+wsx)/2, -H-T/2, 0}, {wsx-wx+2*T, T, 2*W+2*T}, FT);

        addPanel({(wx+wsx)/2, (-H+wsY)/2, -W-T/2}, {wsx-wx+2*T, (-H)-wsY, T}, WT);
        addPanel({(wx+wsx)/2, (-H+wsY)/2,  W+T/2}, {wsx-wx+2*T, (-H)-wsY, T}, WT);
        addPanel({wsx+T/2, (wsY+(-H))/2, 0}, {T, (-H)-wsY, 2*W+2*T}, WT);
        addPanel({wx-T/2,  (wsY+rY)/2,   0}, {T, rY-wsY,   2*W+2*T}, WT);
    }
    addSegWalls({-RX,-SY,-W},{-L,-RY,W}, F_MAXX, T,WT,FT);

    // Asteroids
    auto cluster = [&](Vector3D c, int n, float sx, float sy, float sz, float sMin, float sMax)
    {
        for (int i=0;i<n;i++)
            spawnAsteroid({c.m_x+randF(-sx,sx), c.m_y+randF(-sy,sy), c.m_z+randF(-sz,sz)},
                          randF(sMin,sMax));
    };
    cluster({0,0, 2200}, 4, 140, 80, 140, 2.f, 5.f);  // south room
    cluster({0,0,-2300}, 6, 150, 80, 150, 2.f, 6.f);  // north room
    cluster({0,0,  900}, 3, 120, 70, 500, 2.f, 4.f);  // south arm
    cluster({0,0, -900}, 3, 120, 70, 500, 2.f, 4.f);  // north arm
    cluster({ 1200,0,0}, 3, 500, 70, 120, 2.f, 4.f);  // east arm
    cluster({-1200,0,0}, 3, 500, 70, 120, 2.f, 4.f);  // west arm
    cluster({ 2200,550,0},5, 140, 60, 140, 2.f, 5.f); // upper east room
    cluster({-2200,-550,0},5,140, 60, 140, 2.f, 5.f); // lower west room
}

void DescentGame::addZone(const Vector3D& mn, const Vector3D& mx, int open)
{
    const float R=Ship::kShipRadius;
    TunnelZone z; z.min=mn; z.max=mx;
    if(open&F_MINX) z.min.m_x-=R;
    if(open&F_MAXX) z.max.m_x+=R;
    if(open&F_FLOOR) z.min.m_y-=R;
    if(open&F_CEIL)  z.max.m_y+=R;
    if(open&F_MINZ) z.min.m_z-=R;
    if(open&F_MAXZ) z.max.m_z+=R;
    m_zones.push_back(z);
}

void DescentGame::addSegWalls(const Vector3D& mn, const Vector3D& mx,
                               int open, float T,
                               const wchar_t* wt, const wchar_t* ft)
{
    float cx=(mn.m_x+mx.m_x)*.5f, cy=(mn.m_y+mx.m_y)*.5f, cz=(mn.m_z+mx.m_z)*.5f;
    float sx=mx.m_x-mn.m_x, sy=mx.m_y-mn.m_y, sz=mx.m_z-mn.m_z;
    if(!(open&F_FLOOR)) addPanel({cx, mn.m_y-T*.5f, cz},{sx+2*T,T,sz+2*T},ft);
    if(!(open&F_CEIL))  addPanel({cx, mx.m_y+T*.5f, cz},{sx+2*T,T,sz+2*T},ft);
    if(!(open&F_MINX))  addPanel({mn.m_x-T*.5f,cy,cz},{T,sy,sz+2*T},wt);
    if(!(open&F_MAXX))  addPanel({mx.m_x+T*.5f,cy,cz},{T,sy,sz+2*T},wt);
    if(!(open&F_MINZ))  addPanel({cx,cy,mn.m_z-T*.5f},{sx+2*T,sy,T},wt);
    if(!(open&F_MAXZ))  addPanel({cx,cy,mx.m_z+T*.5f},{sx+2*T,sy,T},wt);
}

void DescentGame::addPanel(const Vector3D& c, const Vector3D& s, const wchar_t* tex)
{
    auto t=createTexture(tex);
    auto mesh=createMesh(L"../Assets/Meshes/box.obj");
    auto mat=createMaterial(L"../Assets/Shaders/base.hlsl");
    mat->addTexture(t);
    mat->setCullMode(CullMode::Front);
    auto e=createEntity<MeshEntity>();
    e->setMesh(mesh); e->addMaterial(mat);
    e->setPosition(c); e->setScale(s);
}

Asteroid* DescentGame::spawnAsteroid(const Vector3D& pos, float scale)
{
    auto* a=createEntity<Asteroid>();
    if(!a) return nullptr;
    a->setPosition(pos);
    a->setScale({scale,scale,scale});
    a->setCollisionRadius(scale*5.5f);
    a->setRotation({randF(0,6.28f),randF(0,6.28f),randF(0,6.28f)});
    return a;
}
