#pragma once

#include <string>

class  ResourceManager;

class  Resource
{
public:
	Resource(const wchar_t* full_path, ResourceManager* manager);
	virtual ~Resource();

	std::wstring getPath();
	
protected:
	std::wstring m_full_path;
	ResourceManager* m_resManager = nullptr;
};
