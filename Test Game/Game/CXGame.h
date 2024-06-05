#pragma once

#include "..\CXPrerequisites.h"
#include "..\Math\CXRect.h"

#include <chrono>
#include <map>
#include <set>
#include <string>

class  CXGame
{
public:
	CXGame();
	virtual ~CXGame();
	
	virtual void onCreate() {}
	virtual void onUpdate(f32 deltaTime) {}
	virtual void onQuit() {}
	
	template <typename T>
	T* createEntity()
	{
		static_assert(std::is_base_of_v<CXEntity, T>, "T must derive from CXEntity");
		auto id = typeid(T).hash_code();
		auto e = new T();
		if (createEntityConcrete(e, id))
		{
			return e;
		}
		return nullptr;
	}
	
	void setTitle(const wchar_t* title);
	void run();
	void quit();
	
	CXInputManager* getInputManager();
	CXGraphicsEngine* getGraphicsEngine();
	CXResourceManager* getResourceManager();

	CXMaterialPtr createMaterial(const wchar_t* path);
	CXTexturePtr createTexture(const wchar_t* path);
	CXMeshPtr createMesh(const wchar_t* path);

private:
	void onDisplaySize(const  CXRect& size);
	//various internal methods of entity system

	//function that allows to update all the data/entities etc... of the game
	void onUpdateInternal();

	//function that allows to update only the graphics part of the game
	void onGraphicsUpdate(f32 deltaTime);

	bool createEntityConcrete(CXEntity* entity, size_t id);
	void removeEntity(CXEntity* entity);

	void updateCamerasViewportSize();
	
	bool m_isRunning = true;

	std::unique_ptr<CXInputManager> m_inputManager;
	std::unique_ptr<CXGraphicsEngine> m_graphicsEngine;
	std::unique_ptr<CXDisplay> m_display; 
	std::unique_ptr<CXResourceManager> m_resourceManager;
	//the core of entity system
	//this map handles all the types of entities we add to the game.
	//for each type of entity (type retrieved with typeid function) we have a map
	//that handles the smart pointers (uniqueptr) of all the entities of that type.
	//By using unique_ptr memory management is handled easily.
	std::map<size_t, std::map<CXEntity*, std::unique_ptr<CXEntity>>> m_entities;

	//m_entitiesToDestroy set handles the entities that have been marked for destroy.
	std::set<CXEntity*> m_entitiesToDestroy;
	//-------------------------------------------------------------------------------
	
	f32 m_avgDt = 0.0f;
	unsigned long long m_avgCount = 0;
	
	std::chrono::system_clock::time_point m_oldTime;
	
	friend class  CXEntity;
	friend class  CXDisplay;
};
