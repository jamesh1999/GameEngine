#include "Engine.h"
#include <iostream>
#include <fstream>

#include <Windows.h>
#include "Clock.h"
#include "ElementFactory.h"
#include "ElementTable.h"
#include "GraphicsController.h"
#include "InputManager.h"
#include "ParticleSystem.h"
#include "ResourceFactory.h"
#include "World.h"

#include "CompositeObject.h"
#include "Renderer.h"
#include "Camera.h"
#include "Light.h"
#include "PropertyDict.h"

using namespace GameEngine;

//Initialise factories with a reference to the engine
Engine::Engine(HINSTANCE hInstance)
{
#ifndef NDEBUG
	//Allocate a console and bind cout/cerr to it
	AllocConsole();
	std::ofstream consoleStream("CONOUT$", std::ios::out);
	std::cout.rdbuf(consoleStream.rdbuf());
	std::cerr.rdbuf(consoleStream.rdbuf());
	std::wofstream wConsoleStream("CONOUT$", std::ios::out);
	std::wcout.rdbuf(wConsoleStream.rdbuf());
#endif


	// Initialise all systems
	elementFactory = new Elements::ElementFactory(this);
	elements = new Elements::ElementTable;
	resourceFactory = new Resources::ResourceFactory(this);
	clock = new GameEngine::Time::Clock;
	input = new Input::InputManager;
	window = new GameEngine::Graphics::Window(hInstance, WndProc, 1000, 1000);
	graphics = new GameEngine::Graphics::GraphicsController(this);
	world = new GameEngine::Elements::World(this);
	resources = new GameEngine::Resources::ResourceTable(this);
	particleSystem = new GameEngine::Graphics::ParticleSystem(this);

	// Attach HandleMessage()
	SetWindowLongPtr(window->GetHandle(), GWLP_USERDATA, (LONG_PTR)this);

	RegisterElement(Elements::CompositeObject);
	// Register default components
	RegisterComponent(Renderer);
	RegisterComponent(Elements::PropertyDict);
	RegisterComponent(Graphics::Light);
	RegisterComponent(Graphics::Camera);
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
	delete elements;
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
	Engine* engine = (Engine*) GetWindowLongPtr(hWnd, GWLP_USERDATA);
	if (engine && engine->HandleMessage(hWnd, message, wParam, lParam)) return message;
	return DefWindowProc(hWnd, message, wParam, lParam);
}
