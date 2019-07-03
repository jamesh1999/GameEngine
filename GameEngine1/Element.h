#ifndef __ELEMENT_INCLUDED__
#define __ELEMENT_INCLUDED__

#include <unordered_set>
#include "Engine.h"
#include "ISerializable.h"

#define RegisterElement(e) (GameEngine::Elements::ElementFactory::elements[typeid(e).name()] = [](){ return static_cast<GameEngine::Elements::Element*>(new e); })

namespace GameEngine
{
	namespace Elements
	{
		class ElementFactory;
		template <class TElement>
		class ElementPtr;

		// An object in the game which is assigned a unique ID
		class Element : public Utils::ISerializable
		{
			friend class ElementFactory;
			template <class>
			friend class ElementPtr;

		public:
			using UID = uint32_t;

		private:
			UID m_uid;
			std::unordered_set<ElementPtr<Element>*> m_ptrs;

		protected:
			Engine* engine = nullptr;

		public:
			// Preferable to constructor because initialisation has finished
			virtual void Create(){};

			virtual ~Element();
			virtual void Destroy();

			UID GetUID();
		};
	}
}

#endif
