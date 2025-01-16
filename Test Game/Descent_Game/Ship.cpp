#include "Ship.h"
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
	
	float speed = 1.0f;
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
		speed = 3.0f;
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
	{/*
		if (turbo)
		{
			if (forward > 0.0f) m_cam_distance = 25.0f;
			else m_cam_distance = 5.0f;
		}
		else
		{
			if (forward > 0.0f) m_cam_distance = 20.0f;
			else m_cam_distance = 9.0f;
		}*/
		
		m_cam_distance = 18.0f;
	}
	else
	{
		//m_cam_distance = 18.0f;
		m_cam_distance = 18.0f;
	}

	auto vec = Vector3D::lerp(Vector3D(m_current_cam_distance,0,0), Vector3D(m_cam_distance,0,0), 2.0f * deltaTime);
	m_current_cam_distance = vec.m_x;


	m_yaw += input->getMouseXAxis() * 0.001f;
	m_pitch += input->getMouseYAxis() * 0.001f;
	
	if (m_pitch < -1.57f)
	{
		m_pitch = -1.57f;
	}
	else if (m_pitch > 1.57f)
	{
		m_pitch = 1.57f;
	}

	auto curr = Vector3D::lerp(Vector3D(m_oldPitch, m_oldYaw, 0.0f), Vector3D(m_pitch, m_yaw, 0.0f), 5.0f * deltaTime);
	m_oldPitch = curr.m_x;
	m_oldYaw = curr.m_y;

	// Roll 
	auto baseRot = getRotation();
	m_roll = m_oldRoll + rotAngle * roll * 0.01f * speed;
	auto currRoll = Vector3D::lerp(Vector3D(0, 0, m_oldRoll), Vector3D(0, 0, m_roll), 5.0f * deltaTime);
	
	m_oldRoll = currRoll.m_z;
	// End of Roll
	
	setRotation(Vector3D(m_oldPitch, m_oldYaw, m_oldRoll));
	std::cout << rightward << '\n';
	//m_camRoll = m_oldRoll;
	
	auto curr_cam = Vector3D::lerp(Vector3D(m_camPitch, m_camYaw, m_camRoll), Vector3D(m_pitch, m_yaw, m_camRoll), 3.0f * deltaTime);
	m_camPitch = curr_cam.m_x;
	m_camYaw = curr_cam.m_y;
	m_camRoll = curr_cam.m_z;

	m_camera->setRotation(Vector3D(m_camPitch, m_camYaw, m_camRoll));

	
	Matrix4x4 w;
	getWorldMatrix(w);
	auto zdir = w.getZDirection();
	auto xdir = w.getXDirection();
	auto ydir = w.getYDirection();
	
	auto pos = m_position + zdir * forward * deltaTime * 100.0f * speed;
	setPosition(pos);
	
	//auto baseRot = getRotation(); 
	//Vector3D rollMove = {baseRot.m_x, baseRot.m_y + rotAngle * roll * deltaTime * 100.0f * speed, baseRot.m_z};
	//setRotation(rollMove);
	
	
	// Camera
	Matrix4x4 w2;
	m_camera->getWorldMatrix(w2);
	 zdir = w2.getZDirection();
	 xdir = w2.getXDirection();
	 ydir = w2.getYDirection();

	
	auto camPos = Vector3D(pos + zdir * -m_current_cam_distance);
	camPos = camPos + ydir * 6.5f;
	
	m_camera->setPosition(camPos);
	
	//On left mouse click, spawn the projectile along the ship direction
	if (input->isMouseUp(MouseButton::Left))
	{
		auto proj = m_game->createEntity<Projectile>();
		proj->m_dir = zdir;
		proj->setPosition(pos);
	}
}
