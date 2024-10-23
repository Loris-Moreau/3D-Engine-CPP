#include <Windows.h>
#include "InputManager.h"

InputManager::InputManager()
{
}

bool InputManager::isKeyDown(const  Key& key)
{
	short keyWin = 0;

	if (key >= Key::A && key <= Key::Z)
	{
		keyWin = 'A' + (static_cast<short>(key) - static_cast<short>(Key::A));
	}
	else if (key >= Key::N0 && key <= Key::N9)
	{
		keyWin = '0' + (static_cast<short>(key) - static_cast<short>(Key::N0));
	}
	else if (key == Key::Shift)
	{
		keyWin = VK_SHIFT;
	}
	else if (key == Key::Escape)
	{
		keyWin = VK_ESCAPE;
	}
	else if (key == Key::Space)
	{
		keyWin = VK_SPACE;
	}

	return  (m_keys_state_res[keyWin] == 0);
}

bool InputManager::isKeyUp(const  Key& key)
{
	short keyWin = 0;

	if (key >= Key::A && key <= Key::Z)
	{
		keyWin = 'A' + (static_cast<short>(key) - 2);
	}
	else if (key >= Key::N0 && key <= Key::N9)
	{
		keyWin = '0' + (static_cast<short>(key) - 28);
	}
	else if (key == Key::Shift)
	{
		keyWin = VK_SHIFT;
	}
	else if (key == Key::Escape)
	{
		keyWin = VK_ESCAPE;
	}
	else if (key == Key::Space)
	{
		keyWin = VK_SPACE;
	}

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

f32 InputManager::getMouseXAxis()
{
	return m_deltaMouse.x;
}

f32 InputManager::getMouseYAxis()
{
	return m_deltaMouse.y;
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

	if (current_mouse_pos.x != static_cast<i32>(m_old_mouse_pos.x) || current_mouse_pos.y != static_cast<i32>(m_old_mouse_pos.y))
	{
		m_deltaMouse = Vec2(static_cast<f32>(current_mouse_pos.x) - (f32)m_old_mouse_pos.x, static_cast<f32>(current_mouse_pos.y) - (f32)m_old_mouse_pos.y);
	}
	else
	{
		m_deltaMouse = Vec2(0, 0);
	}
	
	if (!m_playEnable)
	{
		m_old_mouse_pos = Vec2(static_cast<f32>(current_mouse_pos.x), static_cast<f32>(current_mouse_pos.y));
	}
	else
	{
		Vec2 center_screen = Vec2(m_screenArea.left + static_cast<f32>(m_screenArea.width) / 2.0f, m_screenArea.top + static_cast<f32>(m_screenArea.height) / 2.0f);
		::SetCursorPos(static_cast<i32>(center_screen.x), static_cast<i32>(center_screen.y));
		m_old_mouse_pos = center_screen;
	}
	
	for (unsigned int i = 0; i < 256; i++)
	{
		m_keys_state[i] = ::GetAsyncKeyState(i);
		
		if (m_keys_state[i] & 0x8001)
		{
			m_keys_state_res[i] = 0;
		}
		else // Key is UP
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
