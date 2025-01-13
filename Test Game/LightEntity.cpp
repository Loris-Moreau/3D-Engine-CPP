#include "LightEntity.h"

LightEntity::LightEntity()
{
}

LightEntity::~LightEntity()
{
}

void LightEntity::setColor(const  Vector3D& color)
{
    m_color = color;
}

Vector3D LightEntity::getColor()
{
    return m_color;
}