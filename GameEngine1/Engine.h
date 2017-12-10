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
	}
	namespace ObjectSystem
	{
		class World;
		class ElementFactory;
	}

	class Engine
	{
		//std::list<CompositeObject*> objects;
	public:
		Time::Clock* clock = nullptr;
		Graphics::GraphicsController* graphics = nullptr;
		ObjectSystem::World* world = nullptr;
		ObjectSystem::ElementFactory* elementFactory = nullptr;

		void Loop();
		bool HandleMessage(HWND, UINT, WPARAM, LPARAM);
	};
}

#endif