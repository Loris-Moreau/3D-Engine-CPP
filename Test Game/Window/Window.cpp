#include "Window.h"
#include <cassert>
#include <stdexcept>
#include <Windows.h>

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
			// collected here..

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
		MessageBoxA(hwnd, e.what(), " Error ", MB_ICONERROR | MB_OK);
		PostQuitMessage(0);
	}

	return NULL;
}

Window::Window()
{
	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpszClassName = L" WindowClass";
	wc.lpfnWndProc = &WndProc;

	ATOM classId = 0;
	if (!GetClassInfoEx(HINSTANCE(), L" WindowClass", &wc))
	{
		classId = RegisterClassEx(&wc);
		if (!classId) throw std::runtime_error("RegisterClass failed");
	}

	RECT rc = { 0,0,1024,768 };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	m_hwnd = CreateWindowEx
	(
		NULL, MAKEINTATOM(classId), L"", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		rc.right - rc.left, rc.bottom - rc.top, HWND(), HMENU(), HINSTANCE(), nullptr
	);

	if (!m_hwnd) throw std::runtime_error("CreateWindowEx failed");

	SetWindowLongPtr(static_cast<HWND>(m_hwnd), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));

	ShowWindow(static_cast<HWND>(m_hwnd), SW_SHOW);
	UpdateWindow(static_cast<HWND>(m_hwnd));
}

Rect Window::getClientSize()
{
	RECT rc = {};
	::GetClientRect(static_cast<HWND>(this->m_hwnd), &rc);
	::ClientToScreen(static_cast<HWND>(this->m_hwnd), reinterpret_cast<LPPOINT>(&rc.left));
	::ClientToScreen(static_cast<HWND>(this->m_hwnd), reinterpret_cast<LPPOINT>(&rc.right));
	return {rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top};
}

Rect Window::getScreenSize()
{
	RECT rc = {};

	rc.right = ::GetSystemMetrics(SM_CXSCREEN);
	rc.bottom = ::GetSystemMetrics(SM_CYSCREEN);

	return {rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top};
}

void* Window::getHandle()
{
	return m_hwnd;
}

void Window::setTitle(const wchar_t* title)
{
	::SetWindowText(static_cast<HWND>(m_hwnd), title);
}

Window::~Window()
{
	DestroyWindow(static_cast<HWND>(m_hwnd));
}
