#ifndef __INPUT_HANDLER_INCLUDED__
#define __INPUT_HANDLER_INCLUDED__

#include <Windows.h>

namespace GameEngine
{
	namespace Input
	{
		enum MouseButton
		{
			Left,
			Middle,
			Right
		};

		enum KeyboardButton
		{
			KeyA,
			KeyB,
			KeyC,
			KeyD,
			KeyE,
			KeyF,
			KeyG,
			KeyH,
			KeyI,
			KeyJ,
			KeyK,
			KeyL,
			KeyM,
			KeyN,
			KeyO,
			KeyP,
			KeyQ,
			KeyR,
			KeyS,
			KeyT,
			KeyU,
			KeyV,
			KeyW,
			KeyX,
			KeyY,
			KeyZ,
			NullKey
		};

		class InputManager
		{
		private:
			bool m_pressed[26];
			bool m_mousePressed[3];
			int m_x;
			int m_y;
			int m_mouseLast[2];
			RECT clipRect;

		public:
			InputManager();

			void GetMousePosition(int&, int&);
			bool MouseIsPressed(MouseButton);

			void SetMousePosition(int, int);
			void SetMouseButton(MouseButton, bool);
			void GetMouseDelta(int&, int&);

			bool KeyIsPressed(KeyboardButton);

			void SetKeyboardKey(KeyboardButton, bool);
			KeyboardButton WCharToKeyboard(wchar_t);
			void Update();
			void SetMouseClip(RECT);

			bool HandleMessage(HWND, UINT, WPARAM, LPARAM);
		};
	}
}

#endif
