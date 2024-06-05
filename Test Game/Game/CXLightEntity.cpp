#include "CXLightEntity.h"

CXLightEntity::CXLightEntity()
{
}

CXLightEntity::~CXLightEntity()
{
}

void CXLightEntity::setColor(const  CXVec3& color)
{
	m_color = color;
}

CXVec3 CXLightEntity::getColor()
{
	return m_color;
}
