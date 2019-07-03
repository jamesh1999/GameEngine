#ifndef __GameEngine_INCLUDED__
#define __GameEngine_INCLUDED__

#include <Windows.h>
#include <unordered_map>

namespace GameEngine
{
	namespace Time
	{
		class Clock;
	}

	namespace Graphics
	{
		class GraphicsController;
		class ParticleSystem;
		class Window;
	}

	namespace Elements
	{
		class World;
		class ElementFactory;
		class ElementTable;
	}

	namespace Resources
	{
		class ResourceFactory;
		class ResourceTable;
	}

	namespace Input
	{
		class InputManager;
	}

	class Engine
	{
	public:
		Time::Clock* clock = nullptr;
		Graphics::GraphicsController* graphics = nullptr;
		Graphics::ParticleSystem* particleSystem = nullptr;
		Graphics::Window* window = nullptr;
		Elements::World* world = nullptr;
		Elements::ElementFactory* elementFactory = nullptr;
		Elements::ElementTable* elements = nullptr;
		Resources::ResourceFactory* resourceFactory = nullptr;
		Resources::ResourceTable* resources = nullptr;
		Input::InputManager* input = nullptr;

		Engine(HINSTANCE);

		~Engine();
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		void Loop() const;

		bool HandleMessage(HWND, UINT, WPARAM, LPARAM) const;

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	};
}

#endif
