#pragma once

#include "Prerequisites.h"
#include "Rect.h"
#include "Vector3D.h"

#include <map>
#include <chrono>
#include <set>
#include <string>
#include <vector>

class  Game
{
public:
	Game();
	virtual ~Game();

	virtual void onCreate() {}
	virtual void onUpdate(float deltaTime) {}
	virtual void onQuit() {}

	template <typename T>
	T* createEntity()
	{
		static_assert(std::is_base_of<Entity, T>::value, "T must derive from Entity");
		auto id = typeid(T).hash_code();
		auto e = new T();
		if (createEntityConcrete(e, id))
		{
			return e;
		}
		return nullptr;
	}

	// Returns raw pointers to all live entities of type T.
	// Pointers are valid until the entity is released.
	template <typename T>
	std::vector<T*> getEntitiesOfType()
	{
		static_assert(std::is_base_of<Entity, T>::value, "T must derive from Entity");
		std::vector<T*> result;
		auto id = typeid(T).hash_code();
		auto it = m_entities.find(id);
		if (it != m_entities.end())
		{
			result.reserve(it->second.size());
			for (auto& [key, entityPtr] : it->second)
			{
				result.push_back(static_cast<T*>(entityPtr.get()));
			}
		}
		return result;
	}

	void setTitle(const wchar_t* title);
	void run();
	void quit();

	
	InputManager* getInputManager();
	GraphicsEngine* getGraphicsEngine();
	ResourceManager* getResourceManager();

	MaterialPtr createMaterial(const wchar_t* path);
	TexturePtr createTexture(const wchar_t* path);
	MeshPtr createMesh(const wchar_t* path);

	// Override in derived Game classes to implement world-space constraints (e.g. tunnel walls).
	// Default implementation: no constraint.
	virtual Vector3D constrainPosition(const Vector3D& pos, float radius) const { return pos; }

	// Returns false if pos+radius is outside the level geometry.
	// Projectiles/missiles call this to detect wall hits.
	// Default (open world): always true.
	virtual bool isInBounds(const Vector3D& pos, float radius) const { return true; }

private:
	void onDisplaySize(const  Rect& size);
	// various internal methods of entity system

	// function that allows to update all the data/entities etc... of the game
	void onUpdateInternal();

	// function that allows to update Only the graphics part of the game
	void onGraphicsUpdate(float deltaTime);

	bool createEntityConcrete(Entity* entity, size_t id);
	void removeEntity(Entity* entity);

	void updateCamerasViewportSize();
	
	bool m_isRunning = true;

	std::unique_ptr<InputManager> m_inputManager;
	std::unique_ptr<GraphicsEngine> m_graphicsEngine;
	std::unique_ptr<Display> m_display; 
	std::unique_ptr<ResourceManager> m_resourceManager;
	//the core of entity system ----------------
	//this map handles all the types of entities we add to the game.
	//for each type of entity (type retrieved with typeid function) we have a map
	//that handles the smart pointers (uniqueptr) of all the entities of that type.
	//By using unique_ptr memory management is handled easily.
	std::map<size_t, std::map<Entity*, std::unique_ptr<Entity>>> m_entities;

	//m_entitiesToDestroy set handles the entities that have been marked for destroy.
	std::set<Entity*> m_entitiesToDestroy;
	//------------------------------------------
	float m_avgDt = 0.0f;
	unsigned long long m_avgCount = 0;

	std::chrono::system_clock::time_point m_oldTime;

	friend class  Entity;
	friend class  Display;
};
