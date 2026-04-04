#include "Ship.h"

#include "Missile.h"
#include "Projectile.h"

void Ship::onCreate()
{
    auto tex  = createTexture(L"../Assets/Textures/spaceship.jpg");
    auto mesh = createMesh(L"../Assets/Meshes/spaceship.obj");
    auto mat  = createMaterial(L"../Assets/Shaders/base.hlsl");
    mat->addTexture(tex);

    setMesh(mesh);
    addMaterial(mat);

    // Enable sphere collision
    setCollisionRadius(kShipRadius);

    // Create the camera that follows the spaceship
    m_camera = getGame()->createEntity<CameraEntity>();
    if (m_camera)
        m_camera->setFarPlane(40000.0f);
}

void Ship::onUpdate(float deltaTime)
{
    auto input = getGame()->getInputManager();

    float forward  = 0.0f;
    float rightward = 0.0f;
    float upward   = 0.0f;
    float roll     = 0.0f;
    float rotAngle = 45.0f;
    float speed    = 2.0f;
    bool  turbo    = false;

    // Ship controls
    if (input->isKeyDown(Key::Z)) forward   =  1.0f;
    if (input->isKeyDown(Key::S)) forward   = -1.0f;
    if (input->isKeyDown(Key::Q)) rightward = -1.0f; // strafe left
    if (input->isKeyDown(Key::D)) rightward =  1.0f; // strafe right
    if (input->isKeyDown(Key::Shift)) { speed = 3.5f; turbo = true; }
    if (input->isKeyDown(Key::A)) roll = -1.0f; // roll left
    if (input->isKeyDown(Key::E)) roll =  1.0f; // roll right
    if (input->isKeyDown(Key::Ctrl))  upward = -1.0f; // slide down
    if (input->isKeyDown(Key::Space)) upward =  1.0f; // slide up

    // --- Invincibility timer ---
    if (m_invincibleTime > 0.0f)
        m_invincibleTime -= deltaTime;

    // --- Camera distance spring ---
    if (forward)
    {
        if (turbo) m_cam_distance = (forward > 0.0f) ? 25.0f : 5.0f;
        else       m_cam_distance = (forward > 0.0f) ? 20.0f : 9.0f;
    }
    else
    {
        m_cam_distance = 18.0f;
    }

    Vector3D vec = Vector3D::lerp(
        Vector3D(m_current_cam_distance, 0, 0),
        Vector3D(m_cam_distance,         0, 0),
        2.0f * deltaTime);
    m_current_cam_distance = vec.m_x;

    // --- Rotation ---
    m_yaw   += input->getMouseXAxis() * 0.001f;
    m_pitch += input->getMouseYAxis() * 0.001f;

    m_roll = m_oldRoll + rotAngle * roll * 0.01f * (speed / 2.0f);

    Vector3D curr = Vector3D::lerp(
        Vector3D(m_oldPitch, m_oldYaw, m_oldRoll),
        Vector3D(m_pitch,    m_yaw,    m_roll),
        5.0f * deltaTime);
    m_oldPitch = curr.m_x;
    m_oldYaw   = curr.m_y;
    m_oldRoll  = curr.m_z;

    setRotation(Vector3D(m_oldPitch, m_oldYaw, m_oldRoll));

    m_camPitch = m_oldPitch;
    m_camYaw   = m_oldYaw;

    Vector3D curr_cam = Vector3D::lerp(
        Vector3D(m_oldCamPitch, m_oldCamYaw, m_oldCamRoll),
        Vector3D(m_pitch,       m_yaw,       m_camRoll),
        3.0f * deltaTime);
    m_oldCamPitch = curr_cam.m_x;
    m_oldCamYaw   = curr_cam.m_y;
    m_oldCamRoll  = curr_cam.m_z;

    if (m_camera)
        m_camera->setRotation(Vector3D(m_camPitch, m_camYaw, m_camRoll));

    // --- Movement ---
    Matrix4x4 w;
    getWorldMatrix(w);
    Vector3D xdir = w.getXAxis();
    Vector3D ydir = w.getYAxis();
    Vector3D zdir = w.getZAxis();

    auto pos = m_position
             + (zdir * forward + xdir * rightward + ydir * upward)
             * deltaTime * 100.0f * speed;

    // FIX: clamp position to level geometry via virtual constrainPosition().
    // In open-space games this is a no-op; in tunnel levels it prevents wall clipping.
    pos = getGame()->constrainPosition(pos, kShipRadius);
    setPosition(pos);

    // --- Camera position (FIX: null-check m_camera before use) ---
    if (m_camera)
    {
        Matrix4x4 w2;
        m_camera->getWorldMatrix(w2);
        Vector3D czdir = w2.getZAxis();
        Vector3D cydir = w2.getYAxis();

        Vector3D camPos = pos + czdir * -m_current_cam_distance + cydir * 6.5f;
        m_camera->setPosition(camPos);
    }

    // --- Weapon damage (FIX: compute fresh from base each frame, never accumulate) ---
    m_laserDamage   = kBaseLaserDamage   * (1.0f + (float)(laserLevel   - 1) * 0.1f);
    m_missileDamage = kBaseMissileDamage * (1.0f + (float)(missileLevel - 1) * 0.1f);

    // --- Fire laser (left mouse) ---
    if (input->isMouseUp(MouseButton::Left))
    {
        auto laser = m_game->createEntity<Projectile>();
        if (laser)
        {
            laser->SetDamage(m_laserDamage);
            laser->m_dir = zdir;
            laser->setPosition(pos);
        }
    }

    // --- Fire missile (right mouse) ---
    if (input->isMouseUp(MouseButton::Right))
    {
        if (missileCount < maxMissileCount)
        {
            missileCount++;
            auto missile = m_game->createEntity<Missile>();
            if (missile)
            {
                missile->SetDamage(m_missileDamage);
                missile->m_dir = zdir;
                missile->setPosition(pos);
            }
        }
    }

    // Dev option: refill missiles with middle mouse
    if (input->isMouseDown(MouseButton::Middle))
        ResetMissileCount();
}

void Ship::TakeDamage(float dmg)
{
    if (m_invincibleTime > 0.0f) return; // still invincible
    m_health -= dmg;
    if (m_health < 0.0f) m_health = 0.0f;
    m_invincibleTime = 1.5f; // 1.5 s of grace period
}

void Ship::ResetMissileCount()   { missileCount = 0; }
void Ship::SetMaxMissileCount(unsigned int n) { maxMissileCount = n; }
unsigned int Ship::GetMissileCount() { return missileCount; }

void Ship::SetLaserLevel(unsigned int n)    { laserLevel   = n; }
unsigned int Ship::GetLaserLevel()          { return laserLevel; }
void Ship::SetMissileLevel(unsigned int n)  { missileLevel = n; }
unsigned int Ship::GetMissileLevel()        { return missileLevel; }

float Ship::GetLaserDamage()   const { return m_laserDamage; }
float Ship::GetMissileDamage() const { return m_missileDamage; }
