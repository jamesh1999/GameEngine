#ifndef __INPUT_HANDLER_INCLUDED__
#define __INPUT_HANDLER_INCLUDED__

#include <Windows.h>

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
		static InputManager* instance;

		InputManager();

		static void GetMousePosition(int&, int&);
		static bool MouseIsPressed(MouseButton);

		static void SetMousePosition(int, int);
		static void SetMouseButton(MouseButton, bool);
		static void GetMouseDelta(int&, int&);

		static bool KeyIsPressed(KeyboardButton);

		static void SetKeyboardKey(KeyboardButton, bool);
		static KeyboardButton WCharToKeyboard(wchar_t);
		static void Update();
		static void SetMouseClip(RECT);
	};

	bool HandleMessage(HWND, UINT, WPARAM, LPARAM);
}

#endif
