#ifndef __COMPONENT_INCLUDED__
#define __COMPONENT_INCLUDED__

#include <cstdlib>
#include <malloc.h>
#include <new>
#include "Element.h"

namespace GameEngine
{
	namespace Elements
	{
		class ElementFactory;
		class CompositeObject;

		class alignas(16) Component : public Element
		{
		public:
			friend class ElementFactory;

		private:
			unsigned id = 0;

		public:
			CompositeObject* obj;

			virtual ~Component() {}

			unsigned GetID() const
			{
				return id;
			}

			void* operator new(std::size_t cnt)
			{
				void* ptr = _aligned_malloc(cnt, 16);
				if (!ptr)
					throw std::bad_alloc();
				else
					return ptr;
			}

			void operator delete(void* ptr)
			{
				_aligned_free(ptr);
			}

			//Preferrable to constructor because initialisation has finished
			virtual void Create() {};
		};
	}
}

#endif