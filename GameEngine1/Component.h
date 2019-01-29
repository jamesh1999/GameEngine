#ifndef __COMPONENT_INCLUDED__
#define __COMPONENT_INCLUDED__

#include <cstdlib>
#include <malloc.h>
#include <new>
#include "Element.h"
#include "ElementPtr.h"

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
			ElementPtr<CompositeObject> obj;

			virtual ~Component() = default;

			unsigned GetID() const
			{
				return id;
			}

			void* operator new(size_t cnt)
			{
				void* ptr = _aligned_malloc(cnt, 16);
				if (!ptr)
					throw std::bad_alloc();
				return ptr;
			}

			void operator delete(void* ptr)
			{
				_aligned_free(ptr);
			}

			//Preferable to constructor because initialisation has finished
			virtual void Create() { }

			void Destroy() override;
		};
	}
}

#endif
