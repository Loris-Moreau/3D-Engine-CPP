#pragma once

#include "..\Math\CXRect.h"
#include "..\Resource\CXResource.h"

enum class CXTextureType;

struct CXTextureDesc
{
	CXRect size = {};
	CXTextureType type = CXTextureType::Normal;
};

class  CXTexture : public  CXResource
{
public:
	CXTexture(const wchar_t* full_path, CXResourceManager* manager);
	CXTexture(const CXTextureDesc& desc, CXResourceManager* manager);
	CXTexture2DPtr getTexture2D();
private:
	CXTexture2DPtr m_texture;
	friend class  CXGraphicsEngine;
};

