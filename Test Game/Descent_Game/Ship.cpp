#include "Ship.h"

#include "Missile.h"
#include "Projectile.h"
#include <cmath>

// ---------------------------------------------------------------------------
//  Gram-Schmidt orthonormalise the 3x3 rotation part of a 4x4 matrix.
//  Keeps the forward axis (row 2) authoritative.
// ---------------------------------------------------------------------------
static void orthonormalise(Matrix4x4& m)
{
    // Row 2 = forward (Z), row 1 = up (Y), row 0 = right (X)
    Vector3D fwd(m.mat[2][0], m.mat[2][1], m.mat[2][2]);
    Vector3D up (m.mat[1][0], m.mat[1][1], m.mat[1][2]);

    fwd = Vector3D::normalize(fwd);
    // Re-orthogonalise up against forward
    float dot = Vector3D::dot(up, fwd);
    up = Vector3D::normalize(Vector3D(up.m_x - fwd.m_x*dot,
                                      up.m_y - fwd.m_y*dot,
                                      up.m_z - fwd.m_z*dot));
    // Right = forward cross up  (forward x up = right in LH/DX convention)
    Vector3D right = Vector3D::cross(fwd, up);  // fwd(row2) x up(row1) = right(row0)
    // Verify: right x fwd should give up — but just store results
    // Actually: for LH, X x Y = Z, so fwd(Z) x up(Y) = right(X)?
    // Let's check: Z(0,0,1) x Y(0,1,0) = (0*0-1*1, 1*0-0*0, 0*1-0*0) = (-1,0,0) = -X
    // So we need: right = Vector3D::cross(up, fwd) to get +X from +Y and +Z
    right = Vector3D::cross(up, fwd);

    m.mat[0][0]=right.m_x; m.mat[0][1]=right.m_y; m.mat[0][2]=right.m_z;
    m.mat[1][0]=up.m_x;    m.mat[1][1]=up.m_y;    m.mat[1][2]=up.m_z;
    m.mat[2][0]=fwd.m_x;   m.mat[2][1]=fwd.m_y;   m.mat[2][2]=fwd.m_z;
}

// ---------------------------------------------------------------------------
void Ship::onCreate()
{
    auto tex  = createTexture(L"../Assets/Textures/spaceship.jpg");
    auto mesh = createMesh(L"../Assets/Meshes/spaceship.obj");
    auto mat  = createMaterial(L"../Assets/Shaders/base.hlsl");
    mat->addTexture(tex);
    setMesh(mesh);
    addMaterial(mat);
    setCollisionRadius(kShipRadius);

    m_camera = getGame()->createEntity<CameraEntity>();
    if (m_camera) m_camera->setFarPlane(40000.0f);
}

// ---------------------------------------------------------------------------
void Ship::setInitialYaw(float yaw)
{
    m_orientMat = Matrix4x4::createRotationY(yaw);
}

// ---------------------------------------------------------------------------
void Ship::onUpdate(float dt)
{
    auto input = getGame()->getInputManager();

    // --- Input ---
    float fwd  = 0.f, right = 0.f, up = 0.f, roll = 0.f;
    float speed = 2.f;
    bool  turbo = false;

    if (input->isKeyDown(Key::Z))     fwd   =  1.f;
    if (input->isKeyDown(Key::S))     fwd   = -1.f;
    if (input->isKeyDown(Key::Q))     right = -1.f;
    if (input->isKeyDown(Key::D))     right =  1.f;
    if (input->isKeyDown(Key::Shift)) { speed = 3.5f; turbo = true; }
    if (input->isKeyDown(Key::A))     roll  = -1.f;
    if (input->isKeyDown(Key::E))     roll  =  1.f;
    if (input->isKeyDown(Key::Ctrl))  up    = -1.f;
    if (input->isKeyDown(Key::Space)) up    =  1.f;

    if (m_invincibleTime > 0.f) m_invincibleTime -= dt;

    // -----------------------------------------------------------------------
    //  Rotation — local-space incremental matrix
    //
    //  In this engine's row-vector convention (v' = v*M), a local-space
    //  rotation is achieved by PRE-multiplying the orientation matrix:
    //    M_new = R_local * M_old
    //  This was mathematically verified for all three axes: yaw (Y), pitch (X),
    //  roll (Z) each operate around the ship's OWN axis regardless of current
    //  orientation, so left/right mouse ALWAYS turns the ship left/right in its
    //  current frame — even after roll has been applied.
    // -----------------------------------------------------------------------
    float yawDelta   = input->getMouseXAxis() * 0.0015f;
    float pitchDelta = input->getMouseYAxis() * 0.0015f;
    float rollDelta  = roll * 1.8f * dt;

    if (yawDelta   != 0.f)
        m_orientMat = Matrix4x4::createRotationY(yawDelta)   * m_orientMat;
    if (pitchDelta != 0.f)
        m_orientMat = Matrix4x4::createRotationX(pitchDelta) * m_orientMat;
    if (rollDelta  != 0.f)
        m_orientMat = Matrix4x4::createRotationZ(rollDelta)  * m_orientMat;

    // Orthonormalise periodically to prevent floating-point drift
    m_reorthTimer += dt;
    if (m_reorthTimer > 0.5f) { orthonormalise(m_orientMat); m_reorthTimer = 0.f; }

    // --- Movement ---
    Vector3D xdir(m_orientMat.mat[0][0], m_orientMat.mat[0][1], m_orientMat.mat[0][2]);
    Vector3D ydir(m_orientMat.mat[1][0], m_orientMat.mat[1][1], m_orientMat.mat[1][2]);
    Vector3D zdir(m_orientMat.mat[2][0], m_orientMat.mat[2][1], m_orientMat.mat[2][2]);

    Vector3D newPos = m_position
                    + (zdir * fwd + xdir * right + ydir * up)
                    * dt * 100.f * speed;

    newPos = getGame()->constrainPosition(newPos, kShipRadius);
    m_position = newPos;

    // Build world matrix directly from orientation + scale + translation
    // (bypasses setRotation/processWorldMatrix which use Euler angles)
    m_world = Matrix4x4::createScale(m_scale) * m_orientMat;
    m_world.mat[3][0] = m_position.m_x;
    m_world.mat[3][1] = m_position.m_y;
    m_world.mat[3][2] = m_position.m_z;
    m_world.mat[3][3] = 1.f;

    // --- Camera distance spring ---
    m_cam_distance = fwd ? (turbo ? (fwd>0?25.f:5.f) : (fwd>0?20.f:9.f)) : 18.f;
    m_current_cam_distance += (m_cam_distance - m_current_cam_distance) * 2.f * dt;

    // --- Camera: smooth orientation lag + correct position behind ship ---
    if (m_camera)
    {
        // Blend camera orientation toward ship orientation (exponential lag)
        const float lag = 1.f - expf(-8.f * dt);
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                m_camOrientMat.mat[r][c] += (m_orientMat.mat[r][c] - m_camOrientMat.mat[r][c]) * lag;
        orthonormalise(m_camOrientMat);

        // Camera is behind (−Z) and slightly above (+Y) the ship
        Vector3D camFwd(m_camOrientMat.mat[2][0], m_camOrientMat.mat[2][1], m_camOrientMat.mat[2][2]);
        Vector3D camUp (m_camOrientMat.mat[1][0], m_camOrientMat.mat[1][1], m_camOrientMat.mat[1][2]);
        Vector3D camPos = m_position + camFwd * (-m_current_cam_distance) + camUp * 6.5f;

        // Set camera world matrix directly
        Matrix4x4 camWorld = m_camOrientMat;
        camWorld.mat[3][0] = camPos.m_x;
        camWorld.mat[3][1] = camPos.m_y;
        camWorld.mat[3][2] = camPos.m_z;
        camWorld.mat[3][3] = 1.f;
        // CameraEntity inherits from Entity, m_world is protected — access via setWorldMatrix
        m_camera->setWorldMatrix(camWorld);
    }

    // --- Weapon damage ---
    m_laserDamage   = kBaseLaser   * (1.f + (float)(m_laserLevel   - 1) * 0.1f);
    m_missileDamage = kBaseMissile * (1.f + (float)(m_missileLevel - 1) * 0.1f);

    // --- Fire laser ---
    if (input->isMouseUp(MouseButton::Left))
    {
        auto laser = m_game->createEntity<Projectile>();
        if (laser) { laser->SetDamage(m_laserDamage); laser->m_dir = zdir; laser->setPosition(m_position); }
    }
    // --- Fire missile ---
    if (input->isMouseUp(MouseButton::Right) && missileCount < maxMissileCount)
    {
        missileCount++;
        auto mis = m_game->createEntity<Missile>();
        if (mis) { mis->SetDamage(m_missileDamage); mis->m_dir = zdir; mis->setPosition(m_position); }
    }
    if (input->isMouseDown(MouseButton::Middle)) ResetMissileCount();
}

void Ship::TakeDamage(float dmg)
{
    if (m_invincibleTime > 0.f) return;
    m_health -= dmg;
    if (m_health < 0.f) m_health = 0.f;
    m_invincibleTime = 1.5f;
}

void Ship::ResetMissileCount()               { missileCount = 0; }
void Ship::SetMaxMissileCount(unsigned int n) { maxMissileCount = n; }
unsigned int Ship::GetMissileCount()          { return missileCount; }
void Ship::SetLaserLevel  (unsigned int n)    { m_laserLevel   = n; }
void Ship::SetMissileLevel(unsigned int n)    { m_missileLevel = n; }
