#include "Window.h"

#include <exception>
//Window* window=nullptr;

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
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
			Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (window)
			{
				window->onSize();
			}
			break;
		}
		case WM_SETFOCUS:
		{
			// Event fired when the window get focus
			Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			if (window)
			{
				window->onFocus();
			}
			
			break;
		}
		case WM_KILLFOCUS:
		{
			// Event fired when the window lost focus
			Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			window->onKillFocus();
			
			break;
		}
		case WM_DESTROY:
		{
			// Event fired when the window is destroyed
			Window* window = (Window*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
			window->onDestroy();
			::PostQuitMessage(0);
			
			break;
		}
	
		default:
			return ::DefWindowProc(hwnd, msg, wparam, lparam);
	}

	return NULL;
}

Window::Window()
{
	//Setting up WNDCLASSEX object
	WNDCLASSEX wc;
	wc.cbClsExtra = NULL;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = NULL;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hInstance = nullptr;
	wc.lpszClassName = "MyWindowClass";
	wc.lpszMenuName = "";
	wc.style = NULL;
	wc.lpfnWndProc = &WndProc;

	if (!::RegisterClassEx(&wc)) // If the registration of class fail, the function will return false
	{
		throw std::exception("Window not created successfully");
	}
	
	/*if (!window)
	window = this;*/

	//Creation of the window
	m_hwnd = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MyWindowClass", "DirectX Application",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, nullptr, nullptr, nullptr, nullptr);

	//if the creation fail return false
	if (!m_hwnd)
	{
		throw std::exception("Window not created successfully");
	}
	
	//show up the window
	::ShowWindow(m_hwnd, SW_SHOW);
	::UpdateWindow(m_hwnd);
	
	//set this flag to true to indicate that the window is initialized and running
	m_is_run = true;
}

bool Window::broadcast()
{
	MSG msg;

	if (!this->m_is_init)
	{
		SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
		this->onCreate();
		this->m_is_init = true;
	}

	this->onUpdate();

	while (::PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	Sleep(1);

	return true;
}

bool Window::isRun()
{
	if (m_is_run)
	{
		broadcast();
	}
	return m_is_run;
}

RECT Window::getClientWindowRect()
{
	RECT rc;
	::GetClientRect(this->m_hwnd, &rc);
	return rc;
}

RECT Window::getSizeScreen()
{
	RECT rc;

	rc.right = ::GetSystemMetrics(SM_CXSCREEN);
	rc.bottom = ::GetSystemMetrics(SM_CYSCREEN);

	return rc;
}

void Window::onCreate() {}

void Window::onUpdate() {}

void Window::onDestroy()
{
	m_is_run = false;
}

void Window::onFocus() {}

void Window::onKillFocus() {}

void Window::onSize() {}

Window::~Window() {}
