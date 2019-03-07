#include "Window.h"

using namespace GameEngine::Graphics;

Window::Window(HINSTANCE hInstance, HWND hWnd) : m_hInstance(hInstance), m_hWnd(hWnd)
{
	// Determine window dimensions
	RECT rect;
	GetWindowRect(hWnd, &rect);
	
	m_width = rect.right - rect.left;
	m_height = rect.bottom - rect.top;
}

Window::Window(HINSTANCE hInstance, WNDPROC wndProc, unsigned w, unsigned h) : m_hInstance(hInstance)
{
	//Create the window class
	WNDCLASSEX winClass;
	ZeroMemory(&winClass, sizeof(WNDCLASSEX));

	winClass.cbSize = sizeof(winClass);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = wndProc;
	winClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	winClass.hInstance = hInstance;
	winClass.lpszClassName = L"ENGINE1";

	//Register the window class
	RegisterClassEx(&winClass);

	//Create a window with ENGINE1
	m_hWnd = CreateWindow(
		L"ENGINE1",
		L"Engine",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		w,
		h,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	//Finally show the windows
	ShowWindow(m_hWnd, 10);

	//Try to lock cursor?
	RECT cursorPos = { 400, 400, 500, 500 };
	ShowCursor(false);

	m_width = w;
	m_height = h;
	m_fullscreen = false;
}

// Getters
unsigned Window::GetHeight() { return m_height; }
unsigned Window::GetWidth() { return m_width; }
HWND Window::GetHandle() { return m_hWnd; }
bool Window::GetFullscreen() { return m_fullscreen; }

//Handle window resizing messages
bool Window::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (hWnd != m_hWnd || message != WM_SIZE) return false;
	m_width = LOWORD(lParam);
	m_height = HIWORD(lParam);

	return true;
}