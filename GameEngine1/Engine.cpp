#include "Engine.h"
#include "GraphicsController.h"
#include "Clock.h"
#include "InputManager.h"
#include "World.h"

using namespace GameEngine;

void Engine::Loop()
{
	world->Update();
	Input::InputManager::Update();
	graphics->RenderLightDepth();
	graphics->StartDraw();
	graphics->RenderObjects();
	graphics->EndDraw();
	clock->EndFrame();
	clock->StartFrame();
}

bool GameEngine::Engine::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (graphics->HandleMessage(hWnd, message, wParam, lParam)) return true;

	return false;
}
