#ifndef __WINDOW_INCLUDED__
#define __WINDOW_INCLUDED__

#include <Windows.h>

namespace GameEngine
{
	namespace Graphics
	{
		class Window
		{
		private:
			HINSTANCE m_hInstance;
			HWND m_hWnd;

			bool m_fullscreen;
			unsigned m_width;
			unsigned m_height;

		public:
			Window(HINSTANCE, HWND);
			Window(HINSTANCE, WNDPROC, unsigned, unsigned);
			Window(HINSTANCE hInstance, WNDPROC wndProc) : Window(hInstance, wndProc, 0, 0) {};

			unsigned GetWidth();
			unsigned GetHeight();
			HWND GetHandle();
			bool GetFullscreen();

			bool HandleMessage(HWND, UINT, WPARAM, LPARAM);
		};
	}
}

#endif