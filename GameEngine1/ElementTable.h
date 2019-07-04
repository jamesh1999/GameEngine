#ifndef __ELEMENT_TABLE_INCLUDED__
#define __ELEMENT_TABLE_INCLUDED__

#include <unordered_map>
#include "Element.h"

namespace GameEngine
{
	namespace Elements
	{
		class ElementFactory;

		// Store for all current elements to provide access by UID
		class ElementTable
		{
			friend class Element;
			friend class ElementFactory;

		private:
			std::unordered_map<Element::UID, Element*> m_elements;

		public:
			ElementTable();
			Element* operator[](Element::UID);
		};
	}
}

#endif