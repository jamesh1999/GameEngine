#ifndef __ELEMENT_INCLUDED__
#define __ELEMENT_INCLUDED__

#include <unordered_set>
#include "Engine.h"
#include "ISerializable.h"

#define RegisterElement(e) \
	(GameEngine::Elements::ElementFactory::_RegisterElement( \
		typeid(e).name(), \
		[](){ return (GameEngine::Elements::Element*) new e; } \
	))

namespace GameEngine
{
	namespace Elements
	{
		class ElementFactory;

		// An object in the game which is assigned a unique ID
		class Element : public Utils::ISerializable
		{
			friend class ElementFactory;

		public:
			using UID = uint32_t;

		private:
			UID m_uid;
			Engine* m_engine = nullptr;

		public:
			// Preferable to constructor because initialisation has finished
			virtual void Create(){};

			virtual ~Element();
			virtual void Destroy();

			Engine* GetEngine();
			UID GetUID();
		};
	}
}

#endif
