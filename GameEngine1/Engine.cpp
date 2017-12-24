#include "Engine.h"
#include "GraphicsController.h"
#include "Clock.h"
#include "InputManager.h"
#include "World.h"
#include "ElementFactory.h"

using namespace GameEngine;

Engine::Engine()
{
	elementFactory = new ObjectSystem::ElementFactory;
	elementFactory->engine = this;
}

Engine::~Engine()
{
	if (clock != nullptr)
		delete clock;
	if (graphics != nullptr)
		delete graphics;
	if (world != nullptr)
		delete world;
	if (elementFactory != nullptr)
		delete elementFactory;
}

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
