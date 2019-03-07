#include "InputManager.h"
#include <cstdlib>
#include <windowsx.h>

using namespace GameEngine::Input;

InputManager::InputManager()
{
	for (int i = 0; i < 26; ++i)
		m_pressed[i] = false;
	for (int i = 0; i < 3; ++i)
		m_mousePressed[i] = false;

	m_mouseLast[0] = 0;
	m_mouseLast[1] = 0;
	m_x = 0;
	m_y = 0;
}

void InputManager::GetMousePosition(int& x, int& y)
{
	x = m_x;
	y = m_y;
}

bool InputManager::MouseIsPressed(MouseButton b)
{
	return m_mousePressed[static_cast<int>(b)];
}

void InputManager::SetMousePosition(int x, int y)
{
	m_x = x;
	m_y = y;
}

void InputManager::SetMouseButton(MouseButton b, bool down)
{
	m_mousePressed[static_cast<int>(b)] = down;
}

void InputManager::GetMouseDelta(int& x, int& y)
{
	x = m_x - m_mouseLast[0];
	y = m_y - m_mouseLast[1];
}


bool InputManager::KeyIsPressed(KeyboardButton b)
{
	return m_pressed[static_cast<int>(b)];
}

void InputManager::SetKeyboardKey(KeyboardButton b, bool val)
{
	m_pressed[static_cast<int>(b)] = val;
}

//Convert wchar_t from WM to enum value
KeyboardButton InputManager::WCharToKeyboard(wchar_t b)
{
	char buff[MB_LEN_MAX];
	int result;
	wctomb_s(&result, buff, MB_LEN_MAX, b);
	if (result == 1 && buff[0] >= 'A' && buff[0] <= 'Z')
		return static_cast<KeyboardButton>(buff[0] - 'A');

	return NullKey;
}

void InputManager::Update()
{
	m_mouseLast[0] = m_x;
	m_mouseLast[1] = m_y;
}

//Handle captured windows messages
bool InputManager::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		//Keyboard input
	case WM_KEYDOWN:
		SetKeyboardKey(
			WCharToKeyboard(static_cast<wchar_t>(wParam)), true);
		break;
	case WM_KEYUP:
		SetKeyboardKey(
			WCharToKeyboard(static_cast<wchar_t>(wParam)), false);
		break;

		//Mouse movement
	case WM_MOUSEMOVE:
		SetMousePosition(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

		//Mouse buttons
	case WM_LBUTTONDOWN:
		SetMouseButton(Left, true);
		break;
	case WM_LBUTTONUP:
		SetMouseButton(Left, false);
		break;
	case WM_MBUTTONDOWN:
		SetMouseButton(Middle, true);
		break;
	case WM_MBUTTONUP:
		SetMouseButton(Middle, false);
		break;
	case WM_RBUTTONDOWN:
		SetMouseButton(Right, true);
		break;
	case WM_RBUTTONUP:
		SetMouseButton(Right, false);
		break;

	default:
		//Notify message was not handled
		return false;
	}

	return true;
}
