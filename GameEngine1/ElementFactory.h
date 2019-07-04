#ifndef __ELEMENT_FACTORY_INCLUDED__
#define __ELEMENT_FACTORY_INCLUDED__

#include <random>
#include <type_traits>
#include <unordered_map>
#include <string>
#include "Component.h"
#include "Element.h"
#include "Engine.h"
#include "Transform.h"
#include "ElementTable.h"

namespace GameEngine
{
	namespace Elements
	{
		class CompositeObject;

		//Handles initialisation of Elements
		class ElementFactory
		{
			friend class Engine;

		private:
			Engine* m_engine;

			static std::unordered_map<std::string, Element* (*)()> elements;

			std::random_device rd;
			std::default_random_engine re;
			std::uniform_int_distribution<Element::UID> rDist;

			void Initialise(Element*);
			Element::UID FindFreeUID();

		public:

			ElementFactory(Engine*);

			Engine* GetEngine();

			static void _RegisterElement(std::string, Element* (*)());

			template <class TElement>
			typename TElement* Create();
			Element* Deserialize(std::istream&);
		};

		template <class TElement>
		TElement* ElementFactory::Create()
		{
			static_assert(std::is_base_of<Element, TElement>::value);

			TElement* elem = new TElement;
			Initialise(elem);
			return elem;
		}

		template <>
		CompositeObject* ElementFactory::Create<CompositeObject>();
	}
}

#endif
