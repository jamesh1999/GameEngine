#ifndef __WORLD_INCLUDED__
#define __WORLD_INCLUDED__

#include <list>

class CompositeObject;
namespace GameEngine
{
	namespace ObjectSystem
	{
		class World
		{
		public:
			std::list<CompositeObject*> objects;

		public:
			void Update();
		};
	}
}

#endif