#include "DescentGame.h"
#include "Ship.h"
#include <ctime>

void DescentGame::onCreate()
{
	setTitle(L"DescentGame");

	//Adding SkyBox
	{
		auto tex = createTexture(L"Assets/Textures/stars_map.jpg");
		auto mesh = createMesh(L"Assets/Meshes/sphere.obj");
		auto mat = createMaterial(L"Assets/Shaders/skybox.hlsl");
		mat->addTexture(tex);
		mat->setCullMode(CullMode::Front);

		auto entity = createEntity<MeshEntity>();
		entity->setMesh(mesh);
		entity->addMaterial(mat);
		entity->setScale(Vector3D(20000, 20000, 20000));
	}

	//Adding DLightEntity
	{
		auto entity = createEntity<LightEntity>();
		entity->setColor(Vector3D(1, 1, 1));
		entity->setRotation(Vector3D(-0.707f, 0.707f, 0));
	}

	//Adding DLightEntity
	{
		auto entity = createEntity<LightEntity>();
		entity->setColor(Vector3D(1, 0, 0));
		entity->setRotation(Vector3D(0.707f, 0.707f, 0));
	}

	//Adding Asteroids
	{
		srand((unsigned int)time(nullptr));

		auto tex = createTexture(L"Assets/Textures/asteroid.jpg");
		auto mesh = createMesh(L"Assets/Meshes/asteroid.obj");
		auto mat = createMaterial(L"Assets/Shaders/base.hlsl");
		mat->addTexture(tex);

		for (unsigned int i = 0; i < 200; i++)
		{
			auto entity = createEntity<MeshEntity>();
			entity->setMesh(mesh);
			entity->addMaterial(mat);
			
			entity->setPosition(Vector3D((rand() % 4000) + (-2000), (rand() % 4000) + (-2000), (rand() % 4000) + (-2000)));
			entity->setRotation(Vector3D((rand() % 628) / 100, (rand() % 628) / 100, (rand() % 628) / 100));
			float scale = rand() % 20 + (1.0f);
			entity->setScale(Vector3D(scale, scale, scale));
		}
	}
	//Adding Spaceship
	{
		auto entity = createEntity<Ship>();
	}
	//Enable Play Mode - Cursor locked at the center of screen and invisible
	getInputManager()->enablePlayMode(m_input);
}


void DescentGame::onUpdate(float deltaTime)
{
	if(getInputManager()->isKeyUp(Key::Escape))
	{
		m_input = !m_input;
		getInputManager()->enablePlayMode(m_input);
	}
}

void DescentGame::onQuit()
{
	
}
