#include "Ship.h"

#include "Missile.h"
#include "Projectile.h"

void Ship::onCreate()
{
	//Load all the assets
	auto tex = createTexture(L"../Assets/Textures/spaceship.jpg");
	auto mesh = createMesh(L"../Assets/Meshes/spaceship.obj");
	auto mat = createMaterial(L"../Assets/Shaders/base.hlsl");
	//Add the texture to material
	mat->addTexture(tex);

	//Set the mesh and material into the entity
	setMesh(mesh);
	addMaterial(mat);

	//Create the camera that follows the spaceship
	m_camera = getGame()->createEntity<CameraEntity>();
	m_camera->setFarPlane(40000.0f);
}

void Ship::onUpdate(float deltaTime)
{
	auto input = getGame()->getInputManager();
	
	float forward = 0.0f;
	float rightward = 0.0f;
	float upward = 0.0f; 
	
	float roll = 0.0f;
	float rotAngle = 45.0f;
	
	float speed = 2.0f;
	bool turbo = false;
	
	//Ship controls
	if (input->isKeyDown(Key::Z))
	{
		forward = 1.0f;
	}
	if (input->isKeyDown(Key::S))
	{
		forward = -1.0f;
	}
	if (input->isKeyDown(Key::Q)) // Strafe Left
	{
		rightward = -1.0f;
	}
	if (input->isKeyDown(Key::D)) // Strafe Right 
	{
		rightward = 1.0f;
	}
	if (input->isKeyDown(Key::Shift))
	{
		speed = 3.5f;
		turbo = true;
	}
	// Roll
	if (input->isKeyDown(Key::A)) // roll left
	{
		roll = -1.0f;
	}
	if (input->isKeyDown(Key::E)) // roll right
	{
		roll = 1.0f;
	}
	// Slide
	if (input->isKeyDown(Key::Ctrl)) // Slide Down
	{
		upward = -1.0f;
	}
	if (input->isKeyDown(Key::Space)) // Slide Up
	{
		upward = 1.0f;
	}

	// Handle position and rotation of spaceship and camera
	// With smooth movements, thanks to the lerp function

	if (forward)
	{
		if (turbo)
		{
			if (forward > 0.0f) m_cam_distance = 25.0f;
			else m_cam_distance = 5.0f;
		}
		else
		{
			if (forward > 0.0f) m_cam_distance = 20.0f;
			else m_cam_distance = 9.0f;
		}
		
		//m_cam_distance = 20.0f;
	}
	else
	{
		m_cam_distance = 18.0f;
	}

	Vector3D vec = Vector3D::lerp(Vector3D(m_current_cam_distance,0,0), Vector3D(m_cam_distance,0,0), 2.0f * deltaTime);
	m_current_cam_distance = vec.m_x;


	m_yaw += input->getMouseXAxis() * 0.001f;
	m_pitch += input->getMouseYAxis() * 0.001f;

	// Clamp Pitch (Up/Down)
	/*if (m_pitch < -1.57f)
	{
		m_pitch = -1.57f;
	}
	else if (m_pitch > 1.57f)
	{
		m_pitch = 1.57f;
	}*/

	// TODO :  Current Roll issue (Switch from World rotation to Local Rotation) :
	// 				- Ship Facing Forward -> A Roll = Forward Roll (Pitch)
	// 				- Ship Facing Right -> A Roll = Left Roll 
	// 				- Ship Facing Back -> A Roll = Backwards Roll (inverse Pitch)
	// 				- Ship Facing Left -> A Roll = Right Roll
	
	// Speeeen hehe
	// hehe I think I'm going insane
	// it's 3am and this stupid problem won't go away, AHAHAhahahHAHAa
	m_roll = m_oldRoll + rotAngle * roll * 0.01f * (speed / 2);
	
	Vector3D curr = Vector3D::lerp(Vector3D(m_oldPitch, m_oldYaw, m_oldRoll), Vector3D(m_pitch, m_yaw, m_roll), 5.0f * deltaTime);
	m_oldPitch = curr.m_x;
	m_oldYaw = curr.m_y;
	m_oldRoll = curr.m_z;
	
// slight clamped Roll
	
	//float baseRotZ = getRotation().m_z;
	//m_roll = baseRotZ - m_oldRoll + (rotAngle/2) * roll * 0.01f * speed;
	//Vector3D currRoll = Vector3D::lerp(Vector3D(0, 0, m_oldRoll), Vector3D(0, 0, m_roll), 5.0f * deltaTime);
	//m_oldRoll = currRoll.m_z;
// End of slight clamped Roll
	
	setRotation(Vector3D(m_oldPitch, m_oldYaw, m_oldRoll));
	
	// Debug
	/*
	std::cout << "curr : " << curr.m_x << ", " << curr.m_y << ", " << curr.m_z << '\n';
	std::cout << "currRoll : " << currRoll.m_x << ", " << currRoll.m_y << ", " << currRoll.m_z << '\n';
	*/
	
	m_camPitch = m_oldPitch;
	m_camYaw = m_oldYaw;
	// TODO : Uncomment the line below for release
	//m_camRoll = m_oldRoll;
	
	Vector3D curr_cam = Vector3D::lerp(Vector3D(m_oldCamPitch, m_oldCamYaw, m_oldCamRoll), Vector3D(m_pitch, m_yaw, m_camRoll), 3.0f * deltaTime);
	m_oldCamPitch = curr_cam.m_x;
	m_oldCamYaw = curr_cam.m_y;
	m_oldCamRoll = curr_cam.m_z;
	
	m_camera->setRotation(Vector3D(m_camPitch, m_camYaw, m_camRoll));
	
	// Movement 
	Matrix4x4 w;
	getWorldMatrix(w);
	Vector3D xdir = w.getXAxis();
	Vector3D ydir = w.getYAxis();
	Vector3D zdir = w.getZAxis();
	
	auto pos = m_position + ((zdir * forward) + (xdir * rightward) + (ydir * upward)) * deltaTime * 100.0f * speed;
	setPosition(pos);

	
	// Camera position
	Matrix4x4 w2;
	m_camera->getWorldMatrix(w2);
	zdir = w2.getZAxis();
	xdir = w2.getXAxis();
	ydir = w2.getYAxis();
	
	
	Vector3D camPos = Vector3D(pos + zdir * -m_current_cam_distance);
	camPos = camPos + ydir * 6.5f;
	
	m_camera->setPosition(camPos);
	
	
	if (laserLevel != 1)
	{
		m_laserDamage = m_laserDamage * (1 + ((float)laserLevel / 10.0f));
	}
	if (missileLevel != 1)
	{
		m_missileDamage = m_missileDamage * (1 + ((float)missileLevel / 10.0f));
	}
	
	
	// On left mouse click, spawn the projectile along the ship direction
	if (input->isMouseUp(MouseButton::Left))
	{
		auto laser = m_game->createEntity<Projectile>();
		laser->SetDamage(m_laserDamage);
		laser->m_dir = zdir;
		laser->setPosition(pos);
	}
	// On right mouse click, spawn the missile along the ship direction
	if (input->isMouseUp(MouseButton::Right))
	{
		if (missileCount < maxMissileCount)
		{
			missileCount++;
			
			auto missile = m_game->createEntity<Missile>();
			missile->SetDamage(m_missileDamage);
			missile->m_dir = zdir;
			missile->setPosition(pos );
		}
	}
	
	// TODO : Dev Option (To Remove (or maybe just leave it for shits & giggles))
	if (input->isMouseDown(MouseButton::Middle))
	{
		ResetMissileCount();
	}
	// End of Dev Option
}

void Ship::ResetMissileCount()
{
	missileCount = 0;
}

void Ship::SetMaxMissileCount(unsigned int InMaxCount)
{
	maxMissileCount = InMaxCount;
}

unsigned int Ship::GetMissileCount()
{
	return missileCount;
}

void Ship::SetLaserLevel(unsigned int InLevel)
{
	laserLevel = InLevel;
}

unsigned int Ship::GetLaserLevel()
{
	return laserLevel;
}

void Ship::SetMissileLevel(unsigned int InLevel)
{
	missileLevel = InLevel;
}

unsigned int Ship::GetMissileLevel()
{
	return missileLevel;
}

float Ship::GetLaserDamage()
{
	return m_laserDamage;
}

float Ship::GetMissileDamage()
{
	return m_missileDamage;
}
