#ifndef __ELEMENT_INCLUDED__
#define __ELEMENT_INCLUDED__
#include "Engine.h"
namespace GameEngine
{
	namespace Elements
	{
		class ElementFactory;

		class Element
		{
			friend class ElementFactory;

		protected:
			Engine* engine;
		};
	}
}

#endif