#include "..\Game\CXGame.h"
#include "..\Graphics\CXGraphicsEngine.h"
#include "..\Graphics\CXTexture2D.h"
#include "..\Resource\CXResourceManager.h"
#include "..\Resource\CXTexture.h"
#include <stdexcept>

CXTexture::CXTexture(const wchar_t* full_path, CXResourceManager* manager) : CXResource(full_path, manager)
{
	m_texture = manager->getGame()->getGraphicsEngine()->createTexture2D(full_path);
	if (!m_texture) CX3D_WARNING(L"CXTexture - Dynamic Texture : Creation failed");
}

CXTexture::CXTexture(const CXTextureDesc& desc, CXResourceManager* manager): CXResource(L"",manager)
{
	CXTexture2DDesc desc2d = {};
	desc2d.size = desc.size;
	desc2d.type = desc.type;

	m_texture = manager->getGame()->getGraphicsEngine()->createTexture2D(desc2d);
	if (!m_texture) CX3D_WARNING(L"CXTexture - Dynamic Texture : Creation failed");
}

CXTexture2DPtr CXTexture::getTexture2D()
{
	return m_texture;
}

