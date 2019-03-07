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
Engine::Engine(HINSTANCE hInstance)
{
	elementFactory = new Elements::ElementFactory(this);
	resourceFactory = new Resources::ResourceFactory(this);
	clock = new GameEngine::Time::Clock;
	input = new Input::InputManager;
	window = new GameEngine::Graphics::Window(hInstance, WndProc, 1000, 1000);
	graphics = new GameEngine::Graphics::GraphicsController(this);
	world = new GameEngine::Elements::World;
	resources = new GameEngine::Resources::ResourceTable;
	particleSystem = new GameEngine::Graphics::ParticleSystem(this);

	SetWindowLongPtr(window->GetHandle(), 0, (long)this);
}

//Clean up dynamic allocation
Engine::~Engine()
{
	delete clock;
	delete graphics;
	delete particleSystem;
	delete resources;
	delete world;
	delete input;
	delete window;
	delete elementFactory;
	delete resourceFactory;
}

//One complete loop of the game
void Engine::Loop() const
{
	world->Update();
	particleSystem->Update();
	input->Update();
	graphics->ShadowPasses();
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
	//Chain other message handlers
	if (input != nullptr && input->HandleMessage(hWnd, message, wParam, lParam)) return true;
	if (window != nullptr && window->HandleMessage(hWnd, message, wParam, lParam)) return true;
	if (graphics != nullptr && graphics->HandleMessage(hWnd, message, wParam, lParam)) return true;

	switch (message)
	{
		//Quit the window
	case WM_DESTROY:
		PostQuitMessage(0);
		return true;
	}

	return false;
}

LRESULT CALLBACK Engine::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	Engine* engine = (Engine*) GetWindowLongPtr(hWnd, 0);
	if (engine && engine->HandleMessage(hWnd, message, wParam, lParam)) return message;
	return DefWindowProc(hWnd, message, wParam, lParam);
}
