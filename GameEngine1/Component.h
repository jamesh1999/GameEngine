#ifndef __COMPONENT_INCLUDED__
#define __COMPONENT_INCLUDED__

#include <cstdlib>
#include <malloc.h>
#include <new>

class CompositeObject;

class alignas(16) Component
{
public:
	CompositeObject* obj;
	virtual ~Component() {}

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
};

#endif