#include "..\Game\Game.h"
#include "..\Graphics\GraphicsEngine.h"
#include "..\Graphics\Texture2D.h"
#include "ResourceManager.h"
#include "Texture.h"
#include <stdexcept>

Texture::Texture(const wchar_t* full_path, ResourceManager* manager) : Resource(full_path, manager)
{
	m_texture = manager->getGame()->getGraphicsEngine()->createTexture2D(full_path);
	if (!m_texture) WARNING_3D(L"Texture - Dynamic Texture : Creation failed");
}

Texture::Texture(const TextureDesc& desc, ResourceManager* manager): Resource(L"",manager)
{
	Texture2DDesc desc2d = {};
	desc2d.size = desc.size;
	desc2d.type = desc.type;

	m_texture = manager->getGame()->getGraphicsEngine()->createTexture2D(desc2d);
	if (!m_texture) WARNING_3D(L"Texture - Dynamic Texture : Creation failed");
}

Texture2DPtr Texture::getTexture2D()
{
	return m_texture;
}

