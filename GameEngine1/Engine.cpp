#include "Engine.h"
#include "GraphicsController.h"
#include "Clock.h"
#include "InputManager.h"
#include "World.h"
#include "ElementFactory.h"
#include "ParticleSystem.h"
#include "ResourceFactory.h"

using namespace GameEngine;

//Initialise factories with a reference to the engine
Engine::Engine()
{
	elementFactory = new Elements::ElementFactory;
	elementFactory->engine = this;

	resourceFactory = new Resources::ResourceFactory;
	resourceFactory->engine = this;
}

//Clean up dynamic allocation
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
	if (resourceFactory != nullptr)
		delete resourceFactory;
}

//One complete loop of the game
void Engine::Loop()
{
	world->Update();
	particleSystem->Update();
	Input::InputManager::Update();
	graphics->RenderLightDepth();
	graphics->StartDraw();
	graphics->RenderObjects();
	particleSystem->Draw();
	graphics->EndDraw();
	clock->EndFrame();
	clock->StartFrame();
}

//Handles messages from WndProc()
bool Engine::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (graphics->HandleMessage(hWnd, message, wParam, lParam)) return true;

	return false;
}
