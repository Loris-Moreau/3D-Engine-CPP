/*
 * Ship.cpp
 *
 * Player ship — 6-DOF movement, 3rd-person camera, laser and missile firing.
 *
 * Rotation model (important):
 *   Inputs pre-multiply m_orientMat.  In the engine's row-vector layout
 *   (v' = v*M), pre-multiplying with a local rotation matrix R gives:
 *
 *       M_new = R_local * M_old
 *
 *   This is equivalent to rotating around the ship's OWN axes, so mouse
 *   left/right always turns left/right regardless of the current roll angle.
 *   Using post-multiply instead would give world-space rotation and would
 *   cause the familiar "gimbal-lock" steering problem.
 */

#include "Ship.h"
#include "Missile.h"
#include "Projectile.h"
#include <cmath>

// ---------------------------------------------------------------------------
//  orthonormalise
//
//  Gram-Schmidt re-orthogonalisation of the 3x3 rotation part of a 4x4 matrix.
//  Applied every 0.5 s to counter the tiny floating-point errors that accumulate
//  when many small rotation matrices are multiplied together over thousands of
//  frames.  Without this the axes gradually drift away from being perpendicular
//  and unit-length, causing visible skewing of the ship mesh.
//
//  Row layout (this engine's convention):
//    row 0 = right  (X)
//    row 1 = up     (Y)
//    row 2 = forward(Z)
//
//  Forward is kept authoritative (normalised first).  Up is re-orthogonalised
//  against forward, then right is derived as cross(up, forward).
// ---------------------------------------------------------------------------
static void orthonormalise(Matrix4x4& m)
{
    Vector3D fwd(m.mat[2][0], m.mat[2][1], m.mat[2][2]);
    Vector3D up (m.mat[1][0], m.mat[1][1], m.mat[1][2]);

    fwd = Vector3D::normalize(fwd);

    // Project out any forward component from up, then re-normalise.
    float dot = Vector3D::dot(up, fwd);
    up = Vector3D::normalize({ up.m_x - fwd.m_x*dot,
                                up.m_y - fwd.m_y*dot,
                                up.m_z - fwd.m_z*dot });

    // Right = cross(up, forward).
    // (cross(forward, up) would give −right in a left-handed system.)
    Vector3D right = Vector3D::cross(up, fwd);

    m.mat[0][0] = right.m_x;  m.mat[0][1] = right.m_y;  m.mat[0][2] = right.m_z;
    m.mat[1][0] = up.m_x;     m.mat[1][1] = up.m_y;     m.mat[1][2] = up.m_z;
    m.mat[2][0] = fwd.m_x;    m.mat[2][1] = fwd.m_y;    m.mat[2][2] = fwd.m_z;
}

// ---------------------------------------------------------------------------
//  onCreate
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
    if (m_camera) m_camera->setFarPlane(40000.f);
}

// ---------------------------------------------------------------------------
//  setInitialYaw — sets spawn orientation without fighting the accumulator
// ---------------------------------------------------------------------------
void Ship::setInitialYaw(float yaw)
{
    m_orientMat    = Matrix4x4::createRotationY(yaw);
    m_camOrientMat = m_orientMat;   // camera starts aligned with ship
}

// ---------------------------------------------------------------------------
//  onUpdate — main per-frame ship logic
// ---------------------------------------------------------------------------
void Ship::onUpdate(float dt)
{
    auto input = getGame()->getInputManager();

    // ---- Read input -------------------------------------------------------
    float fwd = 0.f, right = 0.f, up = 0.f, roll = 0.f;
    float speed = 2.f;
    bool  turbo = false;

    if (input->isKeyDown(Key::Z))     fwd   =  1.f;   // thrust forward
    if (input->isKeyDown(Key::S))     fwd   = -1.f;   // thrust backward
    if (input->isKeyDown(Key::Q))     right = -1.f;   // strafe left
    if (input->isKeyDown(Key::D))     right =  1.f;   // strafe right
    if (input->isKeyDown(Key::Shift)) { speed = 3.5f; turbo = true; }
    if (input->isKeyDown(Key::A))     roll  = -1.f;   // roll left
    if (input->isKeyDown(Key::E))     roll  =  1.f;   // roll right
    if (input->isKeyDown(Key::Ctrl))  up    = -1.f;   // slide down
    if (input->isKeyDown(Key::Space)) up    =  1.f;   // slide up

    // ---- Invincibility timer ---------------------------------------------
    if (m_invincibleTime > 0.f) m_invincibleTime -= dt;

    // ---- Rotation (local-space incremental) ------------------------------
    // Each input pre-multiplies m_orientMat with a small local rotation.
    // See the file-level comment for why pre-multiply = local-space rotation.
    const float yawDelta   = input->getMouseXAxis() * 0.0015f;
    const float pitchDelta = input->getMouseYAxis() * 0.0015f;
    const float rollDelta  = roll * 1.8f * dt;

    if (yawDelta   != 0.f) m_orientMat = Matrix4x4::createRotationY(yawDelta)   * m_orientMat;
    if (pitchDelta != 0.f) m_orientMat = Matrix4x4::createRotationX(pitchDelta) * m_orientMat;
    if (rollDelta  != 0.f) m_orientMat = Matrix4x4::createRotationZ(rollDelta)  * m_orientMat;

    // Re-orthonormalise every 0.5 s to prevent floating-point drift.
    m_reorthTimer += dt;
    if (m_reorthTimer > 0.5f) { orthonormalise(m_orientMat); m_reorthTimer = 0.f; }

    // ---- Movement --------------------------------------------------------
    // Extract local axes from the orientation matrix rows.
    const Vector3D xdir(m_orientMat.mat[0][0], m_orientMat.mat[0][1], m_orientMat.mat[0][2]);
    const Vector3D ydir(m_orientMat.mat[1][0], m_orientMat.mat[1][1], m_orientMat.mat[1][2]);
    const Vector3D zdir(m_orientMat.mat[2][0], m_orientMat.mat[2][1], m_orientMat.mat[2][2]);

    Vector3D newPos = m_position
                    + (zdir * fwd + xdir * right + ydir * up)
                    * dt * 100.f * speed;

    // constrainPosition() is a virtual call into DescentGame; it slides the
    // ship along walls instead of stopping it or letting it clip through.
    newPos     = getGame()->constrainPosition(newPos, kShipRadius);
    m_position = newPos;

    // Build the world matrix directly from orientation + scale + translation,
    // bypassing setRotation/processWorldMatrix which use Euler angles.
    m_world = Matrix4x4::createScale(m_scale) * m_orientMat;
    m_world.mat[3][0] = m_position.m_x;
    m_world.mat[3][1] = m_position.m_y;
    m_world.mat[3][2] = m_position.m_z;
    m_world.mat[3][3] = 1.f;

    // ---- Camera distance spring ------------------------------------------
    // Target distance depends on whether the ship is moving and at what speed.
    m_cam_distance = fwd ? (turbo ? (fwd > 0 ? 25.f : 5.f)
                                  : (fwd > 0 ? 20.f : 9.f))
                         : 18.f;
    m_current_cam_distance += (m_cam_distance - m_current_cam_distance) * 2.f * dt;

    // ---- Camera orientation and position ---------------------------------
    if (m_camera)
    {
        // Exponentially interpolate the camera orientation toward the ship's.
        // The lag factor (alpha) controls how closely the camera follows.
        // A smaller alpha = more lag = smoother but slower to react.
        const float alpha = 1.f - expf(-8.f * dt);
        for (int r = 0; r < 3; ++r)
            for (int c = 0; c < 3; ++c)
                m_camOrientMat.mat[r][c] +=
                    (m_orientMat.mat[r][c] - m_camOrientMat.mat[r][c]) * alpha;
        orthonormalise(m_camOrientMat);

        // Place the camera behind (−Z) and slightly above (+Y) the ship.
        const Vector3D camFwd(m_camOrientMat.mat[2][0], m_camOrientMat.mat[2][1], m_camOrientMat.mat[2][2]);
        const Vector3D camUp (m_camOrientMat.mat[1][0], m_camOrientMat.mat[1][1], m_camOrientMat.mat[1][2]);
        const Vector3D camPos = m_position + camFwd * (-m_current_cam_distance) + camUp * 6.5f;

        // Write the camera world matrix directly (setWorldMatrix bypasses the
        // Euler-angle path in processWorldMatrix that would corrupt the result).
        Matrix4x4 camWorld = m_camOrientMat;
        camWorld.mat[3][0] = camPos.m_x;
        camWorld.mat[3][1] = camPos.m_y;
        camWorld.mat[3][2] = camPos.m_z;
        camWorld.mat[3][3] = 1.f;
        m_camera->setWorldMatrix(camWorld);
    }

    // ---- Weapon damage (recalculated each frame from base values) --------
    // Recalculating every frame avoids the old bug where damage accumulated
    // exponentially because it was multiplied in-place each frame.
    m_laserDamage   = kBaseLaser   * (1.f + (float)(m_laserLevel   - 1) * 0.1f);
    m_missileDamage = kBaseMissile * (1.f + (float)(m_missileLevel - 1) * 0.1f);

    // ---- Fire laser (left mouse — up event fires once per click) ---------
    if (input->isMouseUp(MouseButton::Left))
    {
        auto laser = m_game->createEntity<Projectile>();
        if (laser)
        {
            laser->SetDamage(m_laserDamage);
            laser->m_dir = zdir;
            laser->setPosition(m_position);
        }
    }

    // ---- Fire missile (right mouse) -------------------------------------
    if (input->isMouseUp(MouseButton::Right) && missileCount < maxMissileCount)
    {
        missileCount++;
        auto mis = m_game->createEntity<Missile>();
        if (mis)
        {
            mis->SetDamage(m_missileDamage);
            mis->m_dir = zdir;
            mis->setPosition(m_position);
        }
    }

    // Middle mouse refills missiles (dev shortcut, left in for convenience).
    if (input->isMouseDown(MouseButton::Middle)) ResetMissileCount();
}

// ---------------------------------------------------------------------------
void Ship::TakeDamage(float dmg)
{
    if (m_invincibleTime > 0.f) return;   // still in grace period after last hit
    m_health -= dmg;
    if (m_health < 0.f) m_health = 0.f;
    m_invincibleTime = 1.5f;              // 1.5 s of invincibility after each hit
}

void Ship::ResetMissileCount()               { missileCount = 0; }
void Ship::SetMaxMissileCount(unsigned int n) { maxMissileCount = n; }
unsigned int Ship::GetMissileCount()          { return missileCount; }
void Ship::SetLaserLevel  (unsigned int n)    { m_laserLevel   = n; }
void Ship::SetMissileLevel(unsigned int n)    { m_missileLevel = n; }
