#pragma once

#include <vector>
#include "Entity.h"
#include "..\Math\Mat4.h"
#include "..\Math\Rect.h"
#include "..\Math\Vec3.h"
#include "..\Window\Window.h"

class Game;
class  LightEntity : public  Entity
{
public:
	LightEntity();
	virtual ~LightEntity();

	void setColor(const  Vec3& color);
	Vec3 getColor();
	
private:
	Vec3 m_color = Vec3(1, 1, 1);
};
