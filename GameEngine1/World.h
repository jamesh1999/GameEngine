#ifndef __WORLD_INCLUDED__
#define __WORLD_INCLUDED__

#include <list>

namespace GameEngine
{
	namespace Elements
	{
		class CompositeObject;

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