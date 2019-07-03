#ifndef __WORLD_INCLUDED__
#define __WORLD_INCLUDED__

#include <unordered_set>
#include "ISerializable.h"
#include "Engine.h"
#include <istream>
#include <ostream>

namespace GameEngine
{
	namespace Elements
	{
		class CompositeObject;

		class World : public Utils::ISerializable
		{
		public:
			std::unordered_set<CompositeObject*> objects;
			Engine* engine;

		public:

			World(Engine*);

			void Update();

			World& operator<<(std::istream&);
			World& operator>>(std::ostream&);
		};
	}
}

#endif
