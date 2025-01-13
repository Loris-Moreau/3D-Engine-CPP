#include "Window.h"
#include <assert.h>
#include <Windows.h>
#include <stdexcept>

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	try
	{
		//GetWindowLong(hwnd,)
		switch (msg)
		{
		case WM_CREATE:
		{
			// Event fired when the window is created
			// collected here...

			break;
		}
		case WM_SIZE:
		{
			// Event fired when the window is resized
			auto window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			if (window) window->onSize(window->getClientSize());
			break;
		}
		case WM_SETFOCUS:
		{
			// Event fired when the window get focus
			auto window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			if (window) window->onFocus();
			break;
		}
		case WM_KILLFOCUS:
		{
			// Event fired when the window lost focus
			auto window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			window->onKillFocus();
			break;
		}
		case WM_DESTROY:
		{
			// Event fired when the window is destroyed
			auto window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			window->onDestroy();
			//::PostQuitMessage(0);
			break;
		}
		case WM_MOVE:
		{
			// Event fired when the window is destroyed
			auto window = reinterpret_cast<Window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
			if (window) window->onSize(window->getClientSize());
			//::PostQuitMessage(0);
			break;
		}
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			break;
		}

		default:
			return ::DefWindowProc(hwnd, msg, wparam, lparam);
		}
	}
	catch (const std::runtime_error& e)
	{
		MessageBoxA(hwnd, e.what(), "DirectX Game | Error", MB_ICONERROR | MB_OK);
		PostQuitMessage(0);
	}

	return NULL;
}


Window::Window()
{
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpszClassName = (LPCSTR)L" 3DWindow";
	wc.lpfnWndProc = &WndProc;

	ATOM classId = 0;
	if (!GetClassInfoEx(HINSTANCE(), (LPCSTR)L" 3DWindow", &wc))
	{
		classId = RegisterClassEx(&wc);
		if (!classId) throw std::runtime_error("RegisterClassEx failed");
	}

	RECT rc = { 0,0,1024,768 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	m_hwnd = CreateWindowEx(NULL, MAKEINTATOM(classId), (LPCSTR)L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, HWND(), HMENU(), HINSTANCE(), nullptr);

	if (!m_hwnd) throw std::runtime_error("CreateWindowEx failed");

	SetWindowLongPtr((HWND)m_hwnd, GWLP_USERDATA, (LONG_PTR)this);

	ShowWindow((HWND)m_hwnd, SW_SHOW);
	UpdateWindow((HWND)m_hwnd);
}


Rect Window::getClientSize()
{
	RECT rc = {};
	::GetClientRect((HWND)this->m_hwnd, &rc);
	::ClientToScreen((HWND)this->m_hwnd, (LPPOINT)&rc.left);
	::ClientToScreen((HWND)this->m_hwnd, (LPPOINT)&rc.right);
	return Rect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
}

Rect Window::getScreenSize()
{
	RECT rc = {};

	rc.right = ::GetSystemMetrics(SM_CXSCREEN);
	rc.bottom = ::GetSystemMetrics(SM_CYSCREEN);

	return Rect(rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top);
}

void* Window::getHandle()
{
	return m_hwnd;
}

void Window::setTitle(const wchar_t* title)
{
	::SetWindowText((HWND)m_hwnd, (LPCSTR)title);
}

Window::~Window()
{
	DestroyWindow((HWND)m_hwnd);
}