#ifndef __GameEngine_INCLUDED__
#define __GameEngine_INCLUDED__

#include <list>
#include <Windows.h>

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
	}

	namespace Elements
	{
		class World;
		class ElementFactory;
	}

	namespace Resources
	{
		class ResourceFactory;
		class ResourceTable;
	}

	class Engine
	{
		//std::list<CompositeObject*> objects;
	public:
		Time::Clock* clock = nullptr;
		Graphics::GraphicsController* graphics = nullptr;
		Graphics::ParticleSystem* particleSystem = nullptr;
		Elements::World* world = nullptr;
		Elements::ElementFactory* elementFactory = nullptr;
		Resources::ResourceFactory* resourceFactory = nullptr;
		Resources::ResourceTable* resources = nullptr;

		Engine();

		~Engine();
		Engine(const Engine&) = delete;
		Engine& operator=(const Engine&) = delete;

		void Loop();
		bool HandleMessage(HWND, UINT, WPARAM, LPARAM);
	};
}

#endif
