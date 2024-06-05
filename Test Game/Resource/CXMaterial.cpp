//#include "CXGraphicsManager.h"
#include <stdexcept>
#include "..\Game\CXGame.h"
#include "..\Graphics\CXConstantBuffer.h"
#include "..\Graphics\CXGraphicsEngine.h"
#include "CXMaterial.h"
#include "CXResourceManager.h"

CXMaterial::CXMaterial(const wchar_t* full_path, CXResourceManager* manager) : CXResource(full_path, manager)
{
	auto engine = m_resManager->getGame()->getGraphicsEngine();
	m_shader = engine->createShader({ full_path, "vsmain" , full_path, "psmain" });
}

CXMaterial::CXMaterial(const  CXMaterialPtr& material, CXResourceManager* manager) : CXResource(L"", manager)
{
	m_shader = material->m_shader;
}

void CXMaterial::addTexture(const  CXTexturePtr& texture)
{
	m_vec_textures.push_back(texture);
}

void CXMaterial::removeTexture(unsigned int index)
{
	if (index >= this->m_vec_textures.size()) return;
	m_vec_textures.erase(m_vec_textures.begin() + index);
}

void CXMaterial::setData(void* data, unsigned int size)
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

void CXMaterial::setUserData(void* data, unsigned int size)
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

void CXMaterial::setCullMode(const  CXCullMode& mode)
{
	m_cull_mode = mode;
}

CXCullMode CXMaterial::getCullMode()
{
	return m_cull_mode;
}
