#pragma once

#include "Prerequisites.h"
#include "Rect.h"
#include "Vector2D.h"

class  InputManager
{
public:
    InputManager();
    
    virtual bool isKeyDown(const Key& key);
    virtual bool isKeyUp(const Key& key);
    
    virtual bool isMouseDown(const MouseButton& button);
    virtual bool isMouseUp(const MouseButton& button);
    
    virtual float getMouseXAxis();
    virtual float getMouseYAxis();
    
    virtual void enablePlayMode(bool enable);
    
    void setScreenArea(const Rect& area);
    void update();
    
private:
    short m_keys_state[256] = {};
    short m_old_keys_state[256] = {};
    short m_keys_state_res[256] = {};
    
    bool m_playEnable = false;
    Vector2D m_old_mouse_pos;
    bool m_first_time = true;
    Rect m_screenArea;
    Vector2D m_deltaMouse;
    int states_index = 0;
};
