#ifndef __ELEMENT_FACTORY_INCLUDED__
#define __ELEMENT_FACTORY_INCLUDED__

#include <type_traits>
#include "Component.h"
#include "CompositeObject.h"
#include "Element.h"
#include "Engine.h"
#include "Transform.h"

namespace GameEngine
{
	namespace Elements
	{
		//Handles initialisation of Elements i.e. objects within the world heirarchy
		class ElementFactory
		{
			friend class Engine;

		private:
			Engine* engine;
			static int id;

		public:
			template <class T>
			T* Create();
		};
	}
}

template <class T>
T* GameEngine::Elements::ElementFactory::Create()
{
	static_assert(std::is_base_of<Element, T>::value);

	T* elem = new T;
	dynamic_cast<Element*>(elem)->engine = engine;

	//Components just need a unique ID
	dynamic_cast<Component*>(elem)->id = id++;

	return elem;
}

template <>
GameEngine::Elements::CompositeObject* GameEngine::Elements::ElementFactory::Create<GameEngine::Elements::
	CompositeObject>();

#endif
