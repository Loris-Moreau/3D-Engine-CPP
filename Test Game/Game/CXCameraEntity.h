#pragma once

#include "CXEntity.h"
#include "..\Math\CXMat4.h"
#include "..\Math\CXRect.h"
#include "..\Window\CXWindow.h"
#include <vector>

class  CXGame;
class  CXCameraEntity : public  CXEntity
{
public:
	CXCameraEntity();
	virtual ~CXCameraEntity();

	void getViewMatrix(CXMat4& view);
	void getProjectionMatrix(CXMat4& proj);

	void setFarPlane(f32 farPlane);
	void setScreenArea(const CXRect& screen);

private:
	CXMat4 m_view;
	CXMat4 m_projection;

	f32 m_farPlane = 100.0f;
	CXRect m_screenArea;
};
