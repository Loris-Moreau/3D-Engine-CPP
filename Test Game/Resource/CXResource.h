#pragma once

#include <string>

class  CXResourceManager;
class  CXResource
{
public:
	CXResource(const wchar_t* full_path, CXResourceManager* manager);
	virtual ~CXResource();

	std::wstring getPath();
	
protected:
	std::wstring m_full_path;
	CXResourceManager* m_resManager = nullptr;
};
