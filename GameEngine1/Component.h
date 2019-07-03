#ifndef __COMPONENT_INCLUDED__
#define __COMPONENT_INCLUDED__

#include <cstdlib>
#include <malloc.h>
#include <new>
#include "Element.h"
#include "ElementPtr.h"

#define RegisterComponent(c) (RegisterElement(c))

namespace GameEngine
{
	namespace Elements
	{
		class ElementFactory;
		class CompositeObject;

		class alignas(16) Component : public Element
		{
			friend class ElementFactory;

		public:
			ElementPtr<CompositeObject> obj;

			virtual ~Component() = default;

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

			Component& operator>>(std::ostream&);
			Component& operator<<(std::istream&);

			void Destroy() override;
		};
	}
}

#endif
