#include "InputManager.h"
#include "Windows.h"

InputManager::InputManager()
{
}

bool InputManager::isKeyDown(const  Key& key)
{
	short keyWin = 0;
	
	if (key >= Key::Q && key <= Key::Z)
		keyWin = 'Q' + ((short)key - (short)Key::Q);
	else if (key >= Key::N0 && key <= Key::N9)
		keyWin = '0' + ((short)key - (short)Key::N0);
	else if (key == Key::Shift)
		keyWin = VK_SHIFT;
	else if (key == Key::Escape)
		keyWin = VK_ESCAPE;
	else if (key == Key::Space)
		keyWin = VK_SPACE;
	else if (key == Key::Ctrl)
		keyWin = VK_CONTROL;
	return  (m_keys_state_res[keyWin] == 0);
}

bool InputManager::isKeyUp(const  Key& key)
{
	short keyWin = 0;

	if (key >= Key::Q && key <= Key::Z)
		keyWin = 'Q' + ((short)key - 2);
	else if (key >= Key::N0 && key <= Key::N9)
		keyWin = '0' + ((short)key - 28);
	else if (key == Key::Shift)
		keyWin = VK_SHIFT;
	else if (key == Key::Escape)
		keyWin = VK_ESCAPE;
	else if (key == Key::Space)
		keyWin = VK_SPACE;

	return  (m_keys_state_res[keyWin] == 1);
}

bool InputManager::isMouseDown(const  MouseButton& button)
{
	if (button == MouseButton::Left)
	{
		auto state = m_keys_state_res[VK_LBUTTON];
		return (state == 0);
	}
	else if (button == MouseButton::Middle)
	{
		auto state = m_keys_state_res[VK_MBUTTON];
		return (state == 0);
	}
	else if (button == MouseButton::Right)
	{
		auto state = m_keys_state_res[VK_RBUTTON];
		return (state == 0);
	}

	return false;
}

bool InputManager::isMouseUp(const  MouseButton& button)
{
	if (button == MouseButton::Left)
	{
		auto state = m_keys_state_res[VK_LBUTTON];
		return  (state == 1);

	}
	else if (button == MouseButton::Middle)
	{
		auto state = m_keys_state_res[VK_MBUTTON];
		return  (state == 1);
	}
	else if (button == MouseButton::Right)
	{
		auto state = m_keys_state_res[VK_RBUTTON];
		return (state == 1);
	}
	return false;
}

float InputManager::getMouseXAxis()
{
	return m_deltaMouse.m_x;
}

float InputManager::getMouseYAxis()
{
	return m_deltaMouse.m_y;
}

void InputManager::enablePlayMode(bool enable)
{
	m_playEnable = enable;
	ShowCursor(!enable);
}

void InputManager::setScreenArea(const  Rect& area)
{
	m_screenArea = area;
}

void InputManager::update()
{
	POINT current_mouse_pos = {};
	::GetCursorPos(&current_mouse_pos);

	if (current_mouse_pos.x != (int)m_old_mouse_pos.m_x || current_mouse_pos.y != (int)m_old_mouse_pos.m_y)
	{
		m_deltaMouse = Vector2D((float)current_mouse_pos.x - (float)m_old_mouse_pos.m_x, (float)current_mouse_pos.y - (float)m_old_mouse_pos.m_y);
	}
	else
	{
		m_deltaMouse = Vector2D(0, 0);
	}
	
	if (!m_playEnable)
	{
		m_old_mouse_pos = Vector2D((float)current_mouse_pos.x, (float)current_mouse_pos.y);
	}
	else
	{
		Vector2D center_screen = Vector2D((float)m_screenArea.m_left + (float)m_screenArea.m_width / 2.0f, (float)m_screenArea.m_top + (float)m_screenArea.m_height / 2.0f);
		::SetCursorPos((int)center_screen.m_x, (int)center_screen.m_y);
		m_old_mouse_pos = center_screen;
	}
	
	for (unsigned int i = 0; i < 256; i++)
	{
		m_keys_state[i] = ::GetAsyncKeyState((int)i);
		
		if (m_keys_state[i] & 0x8001)
		{
			m_keys_state_res[i] = 0;
		}
		else // Key is Up
		{
			if (m_keys_state[i] != m_old_keys_state[i])
			{
				m_keys_state_res[i] = 1;
			}
			else
			{
				m_keys_state_res[i] = 2;
			}
		}
	}
	
	::memcpy(m_old_keys_state, m_keys_state, sizeof(short) * 256);
}
