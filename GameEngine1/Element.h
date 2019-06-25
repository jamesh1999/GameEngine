#ifndef __ELEMENT_INCLUDED__
#define __ELEMENT_INCLUDED__

#include <unordered_set>
#include "Engine.h"
#include "ISerializable.h"

namespace GameEngine
{
	namespace Elements
	{
		class ElementFactory;
		template <class TElement>
		class ElementPtr;

		class Element : public Utils::ISerializable
		{
			friend class ElementFactory;
			template <class>
			friend class ElementPtr;

		private:
			std::unordered_set<ElementPtr<Element>*> m_ptrs;

		protected:
			Engine* engine = nullptr;

		public:
			virtual ~Element();
			virtual void Destroy();
		};
	}
}

#endif
