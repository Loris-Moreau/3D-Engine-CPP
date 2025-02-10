#include "Game.h"
#include "CameraEntity.h"
#include "Display.h"
#include "GraphicsEngine.h"
#include "InputManager.h"
#include "LightEntity.h"
#include "Material.h"
#include "Matrix4x4.h"
#include "Mesh.h"
#include "MeshEntity.h"
#include "ResourceManager.h"
#include "Texture.h"

#include <Windows.h>

__declspec(align(16))
struct LightData
{
	Vector4D lightColor;
	Vector4D lightDirection;
	Vector4D lightPosition;
};

__declspec(align(16))
struct GraphicsData
{
	Matrix4x4 world;
	Matrix4x4 view;
	Matrix4x4 proj;
	Vector4D cameraPosition;
	LightData lights[32];
	int numLights = 0;
};


Game::Game()
{
	m_inputManager = std::make_unique<InputManager>();
	m_graphicsEngine = std::make_unique<GraphicsEngine>();
	m_display = std::make_unique<Display>(this);
	m_resourceManager = std::make_unique<ResourceManager>(this);

	auto rc = m_display->getClientSize();
	m_inputManager->setScreenArea(rc);
}

Game::~Game()
{
}

void Game::onDisplaySize(const  Rect& size)
{
	updateCamerasViewportSize();

	m_inputManager->setScreenArea(size);

	onUpdateInternal();
}

InputManager* Game::getInputManager()
{
	return m_inputManager.get();
}

GraphicsEngine* Game::getGraphicsEngine()
{
	return m_graphicsEngine.get();
}

ResourceManager* Game::getResourceManager()
{
	return m_resourceManager.get();
}
MaterialPtr Game::createMaterial(const wchar_t* path)
{
	return m_resourceManager->createResourceFromFile<Material>(path);
}
TexturePtr Game::createTexture(const wchar_t* path)
{
	return m_resourceManager->createResourceFromFile<Texture>(path);
}

MeshPtr Game::createMesh(const wchar_t* path)
{
	return m_resourceManager->createResourceFromFile<Mesh>(path);
}

void Game::onGraphicsUpdate(float deltaTime)
{
	m_graphicsEngine->clearColor(
		m_display->getSwapChain(), Vector4D(0, 0, 0, 1));

	m_graphicsEngine->setViewportSize(m_display->getClientSize().m_width,
		m_display->getClientSize().m_height);

	
	GraphicsData gdata;

	// processing cameras data
	{
		auto camId = typeid(CameraEntity).hash_code();
		auto it = m_entities.find(camId);

		// set the camera data to the uniformdata structure, in order to pass them to the shaders for the final rendering
		if (it != m_entities.end())
		{
			for (auto& [key, camera] : it->second)
			{
				Matrix4x4 w;
				auto cam = dynamic_cast<CameraEntity*>(camera.get());
				cam->getViewMatrix(gdata.view);
				cam->getProjectionMatrix(gdata.proj);
				cam->getWorldMatrix(w);
				gdata.cameraPosition = w.getTranslation();
			}
		}
	}


	// processing lighting data
	{
		auto lightId = typeid(LightEntity).hash_code();
		auto it = m_entities.find(lightId);


		// FORWARD RENDERING 
		auto i = 0;
		if (it != m_entities.end())
		{
			for (auto& [key, light] : it->second)
			{
				Matrix4x4 w;
				auto l = dynamic_cast<LightEntity*>(light.get());
				light->getWorldMatrix(w);
				gdata.lights[i].lightDirection = w.getZAxis();
				gdata.lights[i].lightColor = l->getColor();
				i++;
			}
			gdata.numLights = (int)it->second.size();
		}
		
	}


	// processing all the graphic entities
	for (auto& [key, entities] : m_entities)
	{
		// for each graphic entity
		for (auto& [key, entity] : entities)
		{
			if (auto e = dynamic_cast<MeshEntity*>(entity.get()))
			{
				e->getWorldMatrix(gdata.world);
				//let's retrive the world matrix and let's pass it to the uniform buffer
				for (MaterialPtr mat : e->getMaterials())
				{
					mat->setData(&gdata, sizeof(GraphicsData));
				}

				m_graphicsEngine->drawMesh(e->getMesh(), e->getMaterials());
			}
			else
			{
				break;
			}
		}
	}

	m_display->getSwapChain()->present(false);
}

bool Game::createEntityConcrete(Entity* entity, size_t id)
{
	auto entityPtr = std::unique_ptr<Entity>(entity);
	auto camId = typeid(CameraEntity).hash_code();
	entity->m_game = this;
	entity->m_id = id;
	if (id == camId)
	{
		auto it = m_entities.find(camId);
		if (it != m_entities.end())
		{
			if (it->second.size()) return false;
			it->second.emplace(entity, std::move(entityPtr));
		}
		else
		{
			m_entities[camId].emplace(entity, std::move(entityPtr));
		}
	}
	else
	{
		m_entities[id].emplace(entity, std::move(entityPtr));
	}
	entity->onCreate();
	return true;
}

void Game::removeEntity(Entity* entity)
{
	m_entitiesToDestroy.emplace(entity);
}

void Game::updateCamerasViewportSize()
{
	{
		auto camId = typeid(CameraEntity).hash_code();
		auto it = m_entities.find(camId);

		//let's set the camera data to the uniformdata structure, in order to pass them to the shaders for the final rendering
		if (it != m_entities.end())
		{
			for (auto& [key, entity] : it->second)
			{
				auto cam = static_cast<CameraEntity*>(entity.get());
				cam->setScreenArea(m_display->getClientSize());
			}
		}
	}
}

void Game::run()
{
	onCreate();

	updateCamerasViewportSize();
	
	while (m_isRunning)
	{
		MSG msg;
		// WINDOW PROCESSING
		while (PeekMessage(&msg, HWND(), 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				m_isRunning = false;
				continue;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		onUpdateInternal();
	}

	onQuit();
}

void Game::quit()
{
}

void Game::setTitle(const wchar_t* title)
{
	std::wstring t = L"3D Framework - ";
	t += title;
	m_display->setTitle(t.c_str());
}

void Game::onUpdateInternal()
{
	m_inputManager->update();

	//computing delta time-------------------
	//// take the current time
	auto now = std::chrono::system_clock::now();
	// take the diff between the current time and the time taken in the previous frame in seconds (delta time)
	std::chrono::duration<double> elapsedSeconds = now - m_oldTime; 
	if (!m_oldTime.time_since_epoch().count()) // if m_oldTime has not been set yet, simply set elapsedSeconds to 0
	{
		elapsedSeconds = std::chrono::duration<double>();
	}
	m_oldTime = now; // store the current time in order to be used in the next frame



	// computing average delta time (dt more stable)
	// WIP To fix yet.
	m_avgDt += (float)elapsedSeconds.count();
	m_avgCount++;

	float deltaTime = m_avgDt / (float)m_avgCount;

	unsigned long long max = -1;
	if (m_avgCount == 2000)
	{
		m_avgCount = 0;
		m_avgDt = (float)elapsedSeconds.count();
	}
	//---------------------------------------------
	
	// destroy the entities that have been released in the previous iteration
	// destroy the entities here in order to not create issues during the loop over the main map (m_entities)
	//--------------------------------
	for (auto entity : m_entitiesToDestroy)
	{
		auto id = entity->getId();
		m_entities[id].erase(entity);
	}
	m_entitiesToDestroy.clear();
	//----------------------------------

	// update game and entities
	//----------------------------------
	onUpdate(deltaTime);

	for (auto& [key, entities] : m_entities)
	{
		for (auto& [key, entity] : entities)
		{
			entity->onUpdate(deltaTime);
		}
	}
	//----------------------------------

	// update graphics
	onGraphicsUpdate(deltaTime);
}
