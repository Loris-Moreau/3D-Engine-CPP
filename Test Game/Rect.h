#pragma once

#include "Prerequisites.h"

class  Rect
{
public:
    Rect() {}
    Rect(int width, int height) : m_width(width), m_height(height) {}
    Rect(int left, int top, int width, int height) : m_left(left), m_top(top), m_width(width), m_height(height) {}
    Rect(const  Rect& rect) : m_left(rect.m_left), m_top(rect.m_top), m_width(rect.m_width), m_height(rect.m_height) {}
    
    int m_width = 0, m_height = 0, m_left = 0, m_top = 0;
};
