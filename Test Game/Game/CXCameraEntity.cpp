#include "CXCameraEntity.h"
 
CXCameraEntity::CXCameraEntity()
{
}

CXCameraEntity::~CXCameraEntity()
{
}

void CXCameraEntity::getViewMatrix(CXMat4& view)
{
	m_view = m_world;
	m_view.inverse();
	view = m_view;
}

void CXCameraEntity::getProjectionMatrix(CXMat4& proj)
{
	proj = m_projection;
}


void CXCameraEntity::setFarPlane(f32 farPlane)
{
	m_farPlane = farPlane;
	m_projection.setPerspectiveFovLH(1.3f, static_cast<f32>(m_screenArea.width) / static_cast<f32>(m_screenArea.height), 0.1f, m_farPlane);
}

void CXCameraEntity::setScreenArea(const  CXRect& screen)
{
	m_screenArea = screen;
	m_projection.setPerspectiveFovLH(1.3f, static_cast<f32>(m_screenArea.width) / static_cast<f32>(m_screenArea.height), 0.1f, m_farPlane);
}
