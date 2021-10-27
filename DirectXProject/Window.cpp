#include "Window.h"


//Window* window = nullptr;

Window::Window()
{
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg) 
	{
	case WM_CREATE: //EVENT FIRED ON CREATION
	{
		//FETCH CREATED WINDOW
		Window* window = (Window*)((LPCREATESTRUCT)lparam)->lpCreateParams;

		//AND STORE IT FOR LATER
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)window);
		
		window->setHWND(hwnd);
		window->onCreate();
		break;	
	}
	case WM_DESTROY:  //EVENT FIRED ON DESTRUCTION
	{
		//FETCHES THE STORED WINDOW
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

bool Window::init()
{
	// SET UP WNDCLASSEX OBJECT
	WNDCLASSEX wc;
	wc.cbClsExtra = NULL;
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.cbWndExtra = NULL;
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance = NULL;
	wc.lpszClassName = "MyWindow";
	wc.lpszMenuName = "";
	wc.style = NULL;
	wc.lpfnWndProc = &WndProc;


	if (!::RegisterClassEx(&wc))  //RETURN FALSE IF FAILED TO REGISTER
		return false;

	m_hwnd = ::CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, "MyWindow", "DirectX App", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 768, NULL, NULL, NULL, this); // CREATION OF THE WINDOW

	if (!m_hwnd)  //RETURN FALSE IF CREATION FAILED
		return false;

	// DISPLAY WINDOW
	::ShowWindow(m_hwnd, SW_SHOW);
	::UpdateWindow(m_hwnd);

	//SET BOOL TO TRUE TO SHOW WINDOW IS RUNNING
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

	this->onUpdate();

	Sleep(1);

	return true;
}

bool Window::release()
{
	//DESTROY WINDOW, RETURN FALSE IF IT FAILS
	if (!::DestroyWindow(m_hwnd))
		return false;


	return true;
}

bool Window::isRun()
{
	return m_is_run;
}

RECT Window::getClientWindowRect()
{
	RECT rc;
	::GetClientRect(this->m_hwnd, &rc);
	return rc;
}

void Window::setHWND(HWND hwnd)
{
	this->m_hwnd = hwnd;
}

void Window::onCreate()
{
}

void Window::onUpdate()
{
}

void Window::onDestroy()
{
	m_is_run = false;
}

Window::~Window()
{
}
