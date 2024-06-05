#include "CXMaterial.h"
#include "CXMesh.h"
#include "CXResourceManager.h"
#include "CXTexture.h"
#include "CXTexture.h"

#include <filesystem>

CXResourceManager::CXResourceManager(CXGame* game):m_game(game)
{
}

CXResourcePtr CXResourceManager::createResourceFromFileConcrete(const wchar_t* path)
{
	auto it = m_mapResources.find(path);

	std::filesystem::path resPath = path;
	
	auto ext = resPath.extension();

	if (it != m_mapResources.end())
	{
		if (!ext.compare(L".hlsl") || !ext.compare(L".fx"))
		{
			return std::make_shared<CXMaterial>(std::static_pointer_cast<CXMaterial>(it->second), this);
		}
		return it->second;
	}
	
	if (!std::filesystem::exists(resPath)) return CXResourcePtr();
	
	if (!ext.compare(L".jpg") || !ext.compare(L".png") || !ext.compare(L".bmp") || !ext.compare(L".tga"))
	{
		//texture resource
		auto ptr = std::make_shared<CXTexture>(resPath.c_str(), this);
		if (ptr)
		{
			m_mapResources.emplace(path, ptr);
			return ptr;
		}
	}
	else if (!ext.compare(L".obj"))
	{
		//mesh resource
		auto ptr = std::make_shared<CXMesh>(resPath.c_str(), this);
		if (ptr)
		{
			m_mapResources.emplace(path, ptr);
			return ptr;
		}
	}
	else if (!ext.compare(L".hlsl") || !ext.compare(L".fx"))
	{
		//material resource
		auto ptr = std::make_shared<CXMaterial>(resPath.c_str(), this);
		if (ptr)
		{
			m_mapResources.emplace(path, ptr);
			return ptr;
		}
	}

	return CXResourcePtr();
}

CXGame* CXResourceManager::getGame()
{
	return m_game;
}
