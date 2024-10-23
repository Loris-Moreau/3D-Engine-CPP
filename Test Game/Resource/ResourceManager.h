#pragma once

#include <map>
#include <string>
#include "..\Prerequisites.h"
#include "Resource.h"

class  ResourceManager
{
public:
	ResourceManager(Game* game);

	template <typename T>
	std::shared_ptr<T> createResourceFromFile(const wchar_t* path)
	{
		return std::dynamic_pointer_cast<T>(createResourceFromFileConcrete(path));
	}

	Game* getGame();
private:
	ResourcePtr createResourceFromFileConcrete(const wchar_t* path);
	
	std::map<std::wstring, ResourcePtr> m_mapResources;
	Game* m_game = nullptr;
};
