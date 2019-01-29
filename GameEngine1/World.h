#ifndef __WORLD_INCLUDED__
#define __WORLD_INCLUDED__

#include <list>
#include <unordered_set>

namespace GameEngine
{
	namespace Elements
	{
		class CompositeObject;

		class World
		{
		public:
			std::unordered_set<CompositeObject*> objects;

		public:
			void Update();
		};
	}
}

#endif
