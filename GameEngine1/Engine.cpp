#include "Engine.h"
#include "Clock.h"
#include "ElementFactory.h"
#include "GraphicsController.h"
#include "InputManager.h"
#include "ParticleSystem.h"
#include "ResourceFactory.h"
#include "World.h"

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
	delete clock;
	delete graphics;
	delete world;
	delete elementFactory;
	delete resourceFactory;
}

//One complete loop of the game
void Engine::Loop() const
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
bool Engine::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) const
{
	if (graphics != nullptr && graphics->HandleMessage(hWnd, message, wParam, lParam)) return true;

	return false;
}
