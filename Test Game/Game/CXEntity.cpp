#include "..\Game\CXEntity.h"
#include "..\Game\CXGame.h"

CXEntity::CXEntity()
{
}

CXEntity::~CXEntity()
{
}

void CXEntity::release()
{
	m_game->removeEntity(this);
}

void CXEntity::processWorldMatrix()
{
	CXMat4 temp;

	m_world.setIdentity();

	temp.setIdentity();
	temp.setScale(m_scale);
	m_world *= temp;
	
	temp.setIdentity();
	temp.setRotationX(m_rotation.x);
	m_world *= temp;

	temp.setIdentity();
	temp.setRotationY(m_rotation.y);
	m_world *= temp;

	temp.setIdentity();
	temp.setRotationZ(m_rotation.z);
	m_world *= temp;
	
	temp.setIdentity();
	temp.setTranslation(m_position);
	m_world *= temp;
}

size_t CXEntity::getId()
{
	return m_id;
}

void CXEntity::getWorldMatrix(CXMat4& world)
{
	world = m_world;
}

void CXEntity::setPosition(const  CXVec3& position)
{
	m_position = position;
	processWorldMatrix();
}

void CXEntity::setRotation(const  CXVec3& rotation)
{
	m_rotation = rotation;
	processWorldMatrix();
}

void CXEntity::setScale(const  CXVec3& scale)
{
	m_scale = scale;
	processWorldMatrix();
}

CXVec3 CXEntity::getPosition()
{
	return m_position;
}

CXVec3 CXEntity::getRotation()
{
	return m_rotation;
}

CXVec3 CXEntity::getScale()
{
	return m_scale;
}

CXGame* CXEntity::getGame()
{
	return m_game;
}

CXMaterialPtr CXEntity::createMaterial(const wchar_t* path)
{
	return m_game->createMaterial(path);
}

CXTexturePtr CXEntity::createTexture(const wchar_t* path)
{
	return m_game->createTexture(path);
}

CXMeshPtr CXEntity::createMesh(const wchar_t* path)
{
	return m_game->createMesh(path);
}
