#include <Windows.h>
#include "CXInputManager.h"

CXInputManager::CXInputManager()
{
}

bool CXInputManager::isKeyDown(const  CXKey& key)
{
	short keyWin = 0;

	if (key >= CXKey::A && key <= CXKey::Z)
	{
		keyWin = 'A' + (static_cast<short>(key) - static_cast<short>(CXKey::A));
	}
	else if (key >= CXKey::N0 && key <= CXKey::N9)
	{
		keyWin = '0' + (static_cast<short>(key) - static_cast<short>(CXKey::N0));
	}
	else if (key == CXKey::Shift)
	{
		keyWin = VK_SHIFT;
	}
	else if (key == CXKey::Escape)
	{
		keyWin = VK_ESCAPE;
	}
	else if (key == CXKey::Space)
	{
		keyWin = VK_SPACE;
	}

	return  (m_keys_state_res[keyWin] == 0);
}

bool CXInputManager::isKeyUp(const  CXKey& key)
{
	short keyWin = 0;

	if (key >= CXKey::A && key <= CXKey::Z)
	{
		keyWin = 'A' + (static_cast<short>(key) - 2);
	}
	else if (key >= CXKey::N0 && key <= CXKey::N9)
	{
		keyWin = '0' + (static_cast<short>(key) - 28);
	}
	else if (key == CXKey::Shift)
	{
		keyWin = VK_SHIFT;
	}
	else if (key == CXKey::Escape)
	{
		keyWin = VK_ESCAPE;
	}
	else if (key == CXKey::Space)
	{
		keyWin = VK_SPACE;
	}

	return  (m_keys_state_res[keyWin] == 1);
}

bool CXInputManager::isMouseDown(const  CXMouseButton& button)
{
	if (button == CXMouseButton::Left)
	{
		auto state = m_keys_state_res[VK_LBUTTON];
		return (state == 0);

	}
	else if (button == CXMouseButton::Middle)
	{
		auto state = m_keys_state_res[VK_MBUTTON];
		return (state == 0);
	}
	else if (button == CXMouseButton::Right)
	{
		auto state = m_keys_state_res[VK_RBUTTON];
		return (state == 0);
	}

	return false;
}

bool CXInputManager::isMouseUp(const  CXMouseButton& button)
{
	if (button == CXMouseButton::Left)
	{
		auto state = m_keys_state_res[VK_LBUTTON];
		return  (state == 1);
	}
	else if (button == CXMouseButton::Middle)
	{
		auto state = m_keys_state_res[VK_MBUTTON];
		return  (state == 1);
	}
	else if (button == CXMouseButton::Right)
	{
		auto state = m_keys_state_res[VK_RBUTTON];
		return (state == 1);
	}
	
	return false;
}

f32 CXInputManager::getMouseXAxis()
{
	return m_deltaMouse.x;
}

f32 CXInputManager::getMouseYAxis()
{
	return m_deltaMouse.y;
}

void CXInputManager::enablePlayMode(bool enable)
{
	m_playEnable = enable;
	ShowCursor(!enable);
}

void CXInputManager::setScreenArea(const  CXRect& area)
{
	m_screenArea = area;
}

void CXInputManager::update()
{
	POINT current_mouse_pos = {};
	::GetCursorPos(&current_mouse_pos);

	if (current_mouse_pos.x != static_cast<i32>(m_old_mouse_pos.x) || current_mouse_pos.y != static_cast<i32>(m_old_mouse_pos.y))
	{
		m_deltaMouse = CXVec2(static_cast<f32>(current_mouse_pos.x) - (f32)m_old_mouse_pos.x, static_cast<f32>(current_mouse_pos.y) - (f32)m_old_mouse_pos.y);
	}
	else
	{
		m_deltaMouse = CXVec2(0, 0);
	}
	
	if (!m_playEnable)
	{
		m_old_mouse_pos = CXVec2(static_cast<f32>(current_mouse_pos.x), static_cast<f32>(current_mouse_pos.y));
	}
	else
	{
		CXVec2 center_screen = CXVec2(m_screenArea.left + static_cast<f32>(m_screenArea.width) / 2.0f, m_screenArea.top + static_cast<f32>(m_screenArea.height) / 2.0f);
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
