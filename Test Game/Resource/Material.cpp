//#include "GraphicsManager.h"
#include <stdexcept>
#include "..\Game\Game.h"
#include "..\Graphics\ConstantBuffer.h"
#include "..\Graphics\GraphicsEngine.h"
#include "Material.h"
#include "ResourceManager.h"

Material::Material(const wchar_t* full_path, ResourceManager* manager) : Resource(full_path, manager)
{
	auto engine = m_resManager->getGame()->getGraphicsEngine();
	m_shader = engine->createShader({ full_path, "vsmain" , full_path, "psmain" });
}

Material::Material(const  MaterialPtr& material, ResourceManager* manager) : Resource(L"", manager)
{
	m_shader = material->m_shader;
}

void Material::addTexture(const  TexturePtr& texture)
{
	m_vec_textures.push_back(texture);
}

void Material::removeTexture(unsigned int index)
{
	if (index >= this->m_vec_textures.size()) return;
	m_vec_textures.erase(m_vec_textures.begin() + index);
}

void Material::setData(void* data, unsigned int size)
{
	auto engine = m_resManager->getGame()->getGraphicsEngine();

	if (!m_constant_buffer)
	{
		m_constant_buffer = engine->createConstantBuffer({ data, size });
	}
	else
	{
		m_constant_buffer->update(data);
	}
}

void Material::setUserData(void* data, unsigned int size)
{
	auto engine = m_resManager->getGame()->getGraphicsEngine();

	if (!m_userBuffer)
	{
		m_userBuffer = engine->createConstantBuffer({ data, size });
	}
	else
	{
		m_userBuffer->update(data);
	}
}

void Material::setCullMode(const  CullMode& mode)
{
	m_cull_mode = mode;
}

CullMode Material::getCullMode()
{
	return m_cull_mode;
}
