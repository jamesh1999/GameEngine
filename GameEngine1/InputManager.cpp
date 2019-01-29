#include "InputManager.h"
#include <cstdlib>
#include <windowsx.h>

using namespace Input;

InputManager* InputManager::instance = nullptr;

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

	instance = this;
}

void InputManager::GetMousePosition(int& x, int& y)
{
	x = instance->m_x;
	y = instance->m_y;
}

bool InputManager::MouseIsPressed(MouseButton b)
{
	return instance->m_mousePressed[static_cast<int>(b)];
}

void InputManager::SetMousePosition(int x, int y)
{
	instance->m_x = x;
	instance->m_y = y;
}

void InputManager::SetMouseButton(MouseButton b, bool down)
{
	if (instance == nullptr) return;
	instance->m_mousePressed[static_cast<int>(b)] = down;
}

void InputManager::GetMouseDelta(int& x, int& y)
{
	x = instance->m_x - instance->m_mouseLast[0];
	y = instance->m_y - instance->m_mouseLast[1];
}


bool InputManager::KeyIsPressed(KeyboardButton b)
{
	return instance->m_pressed[static_cast<int>(b)];
}

void InputManager::SetKeyboardKey(KeyboardButton b, bool val)
{
	if (instance == nullptr) return;
	instance->m_pressed[static_cast<int>(b)] = val;
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
	instance->m_mouseLast[0] = instance->m_x;
	instance->m_mouseLast[1] = instance->m_y;
}

//Handle captured windows messages
bool Input::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		//Keyboard input
	case WM_KEYDOWN:
		InputManager::SetKeyboardKey(
			InputManager::WCharToKeyboard(static_cast<wchar_t>(wParam)), true);
		break;
	case WM_KEYUP:
		InputManager::SetKeyboardKey(
			InputManager::WCharToKeyboard(static_cast<wchar_t>(wParam)), false);
		break;

		//Mouse movement
	case WM_MOUSEMOVE:
		InputManager::SetMousePosition(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		break;

		//Mouse buttons
	case WM_LBUTTONDOWN:
		InputManager::SetMouseButton(Left, true);
		break;
	case WM_LBUTTONUP:
		InputManager::SetMouseButton(Left, false);
		break;
	case WM_MBUTTONDOWN:
		InputManager::SetMouseButton(Middle, true);
		break;
	case WM_MBUTTONUP:
		InputManager::SetMouseButton(Middle, false);
		break;
	case WM_RBUTTONDOWN:
		InputManager::SetMouseButton(Right, true);
		break;
	case WM_RBUTTONUP:
		InputManager::SetMouseButton(Right, false);
		break;

		//Notify message was not handled
	default:
		return false;
	}

	return true;
}
