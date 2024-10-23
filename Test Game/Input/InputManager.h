#pragma once

#include "..\Prerequisites.h"
#include "..\Math\Rect.h"
#include "..\Math\Vec2.h"

class  InputManager
{
public:
	InputManager();

	virtual bool isKeyDown(const  Key& key);
	virtual bool isKeyUp(const  Key& key);

	virtual bool isMouseDown(const  MouseButton& button);
	virtual bool isMouseUp(const  MouseButton& button);

	virtual f32 getMouseXAxis();
	virtual f32 getMouseYAxis();

	virtual void enablePlayMode(bool enable);

	void setScreenArea(const  Rect& area);
	
	void update();

private:
	short m_keys_state[256] = {};
	short m_old_keys_state[256] = {};
	short m_keys_state_res[256] = {};

	bool m_playEnable = false;
	bool m_first_time = true;
	
	Rect m_screenArea;
	
	Vec2 m_old_mouse_pos;
	Vec2 m_deltaMouse;
	
	int states_index = 0;
};
