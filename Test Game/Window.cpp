#include "Window.h"

Window::Window()
{

}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	  case WM_CREATE:
	{
		// Event fired when the window is created
		// collected here..
		Window* window = (Window*)((LPCREATESTRUCT)lparam)->lpCreateParams;
		// .. and then stored for later lookup
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
		window->OnCreate();
		break;
	}
  
  	case WM_DESTROY:
	{
		// Event fired when the window is destroyed
		Window* window =(Window*) GetWindowLongPtr(hwnd, GWLP_USERDATA);
		window->OnDestroy();
		::PostQuitMessage(0);
		break;
	}

	 default:
		 return ::DefWindowProc(hwnd, msg, wparam, lparam);
	
	}

	return NULL;
}

bool Window::init()
{
	//settings of WindClassEx object
		WNDCLASSEX wc;

		wc.cbClsExtra = NULL;
		wc.cbSize = sizeof(WNDCLASSEX);
		wc.cbWndExtra = NULL;

		wc.hbrBackground = (HBRUSH)COLOR_WINDOW;

		wc.hCursor = LoadCursor(NULL, IDC_ARROW);

		wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		wc.hInstance = NULL;

		wc.lpszClassName = L"MyWindowClass";
		wc.lpszMenuName = L"";

		wc.style = NULL;

		wc.lpfnWndProc = &WndProc;


	if (!::RegisterClassEx(&wc)) //if the registration of the class fails
		return false;



	//creation of the window
	m_hwnd = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, wc.lpszClassName, L"TestApplication", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, NULL, NULL, NULL, this);
	
	if (!m_hwnd)
		return false;

	//show the window
	::ShowWindow(m_hwnd, SW_SHOW);

	::UpdateWindow(m_hwnd);


	m_is_run = true;
	return true;
}

bool Window::broadcast()
{
	MSG msg;

	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	this->OnUpdate();

	Sleep(0);

	return true;
}

bool Window::release()
{
	//Destroy/Remove the window
	if (!::DestroyWindow(m_hwnd))
		return false;

	return true;
}

bool Window::isRun()
{
	return m_is_run;
}

void Window::OnCreate()
{

}

void Window::OnUpdate()
{

}

void Window::OnDestroy()
{  
	m_is_run = false;
}

Window::~Window()
{

}
