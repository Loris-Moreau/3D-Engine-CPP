#pragma once

#include <map>
#include <string>
#include "..\CXPrerequisites.h"
#include "..\Resource\CXResource.h"

class  CXResourceManager
{
public:
	CXResourceManager(CXGame* game);

	template <typename T>
	std::shared_ptr<T> createResourceFromFile(const wchar_t* path)
	{
		return std::dynamic_pointer_cast<T>(createResourceFromFileConcrete(path));
	}

	CXGame* getGame();
private:
	CXResourcePtr createResourceFromFileConcrete(const wchar_t* path);
	
	std::map<std::wstring, CXResourcePtr> m_mapResources;
	CXGame* m_game = nullptr;
};
