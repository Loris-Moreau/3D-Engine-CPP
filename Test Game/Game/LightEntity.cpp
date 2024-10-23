#include "LightEntity.h"

LightEntity::LightEntity()
{
}

LightEntity::~LightEntity()
{
}

void LightEntity::setColor(const  Vec3& color)
{
	m_color = color;
}

Vec3 LightEntity::getColor()
{
	return m_color;
}
