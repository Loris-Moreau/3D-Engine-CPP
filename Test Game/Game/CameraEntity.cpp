#include "CameraEntity.h"
 
CameraEntity::CameraEntity()
{
}

CameraEntity::~CameraEntity()
{
}

void CameraEntity::getViewMatrix(Mat4& view)
{
	m_view = m_world;
	m_view.inverse();
	view = m_view;
}

void CameraEntity::getProjectionMatrix(Mat4& proj)
{
	proj = m_projection;
}


void CameraEntity::setFarPlane(f32 farPlane)
{
	m_farPlane = farPlane;
	m_projection.setPerspectiveFovLH(1.3f, static_cast<f32>(m_screenArea.width) / static_cast<f32>(m_screenArea.height), 0.1f, m_farPlane);
}

void CameraEntity::setScreenArea(const  Rect& screen)
{
	m_screenArea = screen;
	m_projection.setPerspectiveFovLH(1.3f, static_cast<f32>(m_screenArea.width) / static_cast<f32>(m_screenArea.height), 0.1f, m_farPlane);
}
