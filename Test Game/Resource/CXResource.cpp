#include "CXResource.h"
#include "CXResourceManager.h"

CXResource::CXResource(const wchar_t* full_path, CXResourceManager* manager) : m_full_path(full_path), m_resManager(manager)
{
}

CXResource::~CXResource()
{
}

std::wstring CXResource::getPath()
{
	return m_full_path;
}
