#include "CameraEntity.h"

void CameraEntity::getViewMatrix(Matrix4x4& view)
{

    m_view = m_world;
    m_view.inverse();
    view = m_view;
}

void CameraEntity::getProjectionMatrix(Matrix4x4& proj)
{
    proj = m_projection;
}


void CameraEntity::setFarPlane(float farPlane)
{
    m_farPlane = farPlane;
    m_projection.setPerspectiveFovLH(1.3f, (float)m_screenArea.m_width / (float)m_screenArea.m_height, 0.1f, m_farPlane);
}

void CameraEntity::setScreenArea(const  Rect& screen)
{
    m_screenArea = screen;
    m_projection.setPerspectiveFovLH(1.3f, (float)m_screenArea.m_width / (float)m_screenArea.m_height, 0.1f, m_farPlane);
}