#pragma once

#include "Entity.h"
#include "Matrix4x4.h"
#include "Rect.h"
#include "Vector3D.h"
#include "Window.h"

#include <vector>

class Game;
class  LightEntity : public  Entity
{
public:
    LightEntity();
    virtual ~LightEntity();

    void setColor(const  Vector3D& color);
    Vector3D getColor();
    
private:
    Vector3D m_color = Vector3D(1, 1, 1);
};
