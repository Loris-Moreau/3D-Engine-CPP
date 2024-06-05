#pragma once

#include "..\CXPrerequisites.h"
#include "..\Math\CXRect.h"
#include "..\Math\CXVec2.h"

class  CXInputManager
{
public:
	CXInputManager();

	virtual bool isKeyDown(const  CXKey& key);
	virtual bool isKeyUp(const  CXKey& key);

	virtual bool isMouseDown(const  CXMouseButton& button);
	virtual bool isMouseUp(const  CXMouseButton& button);

	virtual f32 getMouseXAxis();
	virtual f32 getMouseYAxis();

	virtual void enablePlayMode(bool enable);

	void setScreenArea(const  CXRect& area);
	
	void update();

private:
	short m_keys_state[256] = {};
	short m_old_keys_state[256] = {};
	short m_keys_state_res[256] = {};

	bool m_playEnable = false;
	bool m_first_time = true;
	
	CXRect m_screenArea;
	
	CXVec2 m_old_mouse_pos;
	CXVec2 m_deltaMouse;
	
	int states_index = 0;
};
