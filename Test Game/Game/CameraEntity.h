#pragma once

#include "Entity.h"
#include "..\Math\Mat4.h"
#include "..\Math\Rect.h"
#include "..\Window\Window.h"
#include <vector>

class  Game;
class  CameraEntity : public  Entity
{
public:
	CameraEntity();
	virtual ~CameraEntity();

	void getViewMatrix(Mat4& view);
	void getProjectionMatrix(Mat4& proj);

	void setFarPlane(f32 farPlane);
	void setScreenArea(const Rect& screen);

private:
	Mat4 m_view;
	Mat4 m_projection;

	f32 m_farPlane = 100.0f;
	Rect m_screenArea;
};
