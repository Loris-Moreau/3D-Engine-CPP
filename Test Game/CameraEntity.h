#pragma once

#include <vector>

#include "Entity.h"
#include "Matrix4x4.h"
#include "Rect.h"
#include "Window.h"

class  Game;

class  CameraEntity : public  Entity
{
public:
    CameraEntity() = default;
    virtual ~CameraEntity() override = default;

    void getViewMatrix(Matrix4x4& view);
    void getProjectionMatrix(Matrix4x4& proj);

    void setFarPlane(float farPlane);
    void setScreenArea(const  Rect& screen);

private:
    Matrix4x4 m_view;
    Matrix4x4 m_projection;

    float m_farPlane = 100.0f;
    Rect m_screenArea;
};