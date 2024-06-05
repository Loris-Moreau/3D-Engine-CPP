#pragma once

#include <vector>
#include "CXEntity.h"
#include "..\Math\CXMat4.h"
#include "..\Math\CXRect.h"
#include "..\Math\CXVec3.h"
#include "..\Window\CXWindow.h"

class Game;
class  CXLightEntity : public  CXEntity
{
public:
	CXLightEntity();
	virtual ~CXLightEntity();

	void setColor(const  CXVec3& color);
	CXVec3 getColor();
	
private:
	CXVec3 m_color = CXVec3(1, 1, 1);
};
