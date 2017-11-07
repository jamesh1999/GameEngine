#ifndef __GCPOINTER_INCLUDED__
#define __GCPOINtER_INCLUDED__

#include <type_traits>
#include <exception>
#include "GCObject.h"

class GarbageCollector;

template <class T>
class GCPointer
{
	friend class GarbageCollector;

private:
	T* m_object = nullptr;
	GCObject* m_parent = nullptr;

	bool m_initialized = false;

public:

	GCPointer()
	{
		if (!m_initialized) return;
		
		throw std::exception("GCPointer already initialized");
	}

	explicit GCPointer(GCObject* parent)
	{
		if(m_initialized)
			throw std::exception("GCPointer already initialized");

		m_parent = parent;
		m_parent->childGCPs.push_back(reinterpret_cast<GCPointer<void>*>(this));

		m_initialized = true;
	}

	~GCPointer()
	{
		if (!m_initialized) return;

		m_parent->childGCPs.remove(reinterpret_cast<GCPointer<void>*>(this));
	}

	GCPointer& operator=(T* gcObject)
	{
		static_assert(std::is_base_of<GCObject, std::remove_pointer<T>>::value, "");
		m_object = gcObject;

		return *this;
	}
};

#define InitGCPointer(gcp) (gcp = GCPointer(dynamic_cast<GCObject*>(this)))

#endif
