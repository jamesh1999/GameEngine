#ifndef __GCPOINTER_INCLUDED__
#define __GCPOINTER_INCLUDED__

#include <type_traits>
#include <exception>
#include <cassert>
#include "GCObject.h"

class GarbageCollector;

template <class T>
class GCPointer
{
	friend class GarbageCollector;

private:
	mutable T* m_object = nullptr;
	GCObject* m_parent = nullptr;

#ifndef NDEBUG
	bool m_initialized = false;
#endif

public:

	GCPointer() = default;

	explicit GCPointer(GCObject* parent, T* object)
	{
		static_assert(std::is_base_of<GCObject, T>::value, "");

		m_object = object;
		m_parent = parent;
		m_parent->childGCPs.push_back(reinterpret_cast<GCPointer<GCObject>*>(this));

#ifndef NDEBUG
		m_initialized = true;
#endif
	}

	explicit GCPointer(GCObject* parent) : GCPointer(parent, nullptr) {};

	~GCPointer()
	{
#ifndef NDEBUG
		assert(m_initialized);
#endif
		m_parent->childGCPs.remove(reinterpret_cast<GCPointer<GCObject>*>(this));
	}

	GCPointer(const GCPointer<T>& other)
	{
#ifndef NDEBUG
		assert(other.m_initialized);
#endif
		m_object = other.m_object;
		m_parent = other.m_parent;
		m_parent->childGCPs.push_back(reinterpret_cast<GCPointer<GCObject>*>(this));
#ifndef NDEBUG
		m_initialized = true;
#endif
	}

	GCPointer& operator=(const GCPointer<T>& other)
	{
#ifndef NDEBUG
		assert(m_initialized);
		assert(other.m_initialized);
#endif
		m_object = other.m_object;
		return *this;
	}

	GCPointer(GCPointer<T>&& other)
	{
#ifndef NDEBUG
		assert(other.m_initialized);
#endif
		m_object = other.m_object;
		m_parent = other.m_parent;
		m_parent->childGCPs.push_back(reinterpret_cast<GCPointer<GCObject>*>(this));
#ifndef NDEBUG
		m_initialized = true;
#endif
	}

	GCPointer& operator=(GCPointer<T>&& other)
	{
#ifndef NDEBUG
		assert(m_initialized);
		assert(other.m_initialized);
#endif
		m_object = other.m_object;
		return *this;
	}

	GCPointer& operator=(T* gcObject)
	{
#ifndef NDEBUG
		assert(m_initialized);
#endif
		m_object = gcObject;
		return *this;
	}

	void _Initialize(GCObject* parent)
	{
		m_parent = parent;
#ifndef NDEBUG
		m_initialized = true;
#endif
	}

	T* Get() const
	{
		// Break dead references
		if (m_object == nullptr || m_object->m_alive) return m_object;
		m_object = nullptr;
		return m_object;
	}

	T* operator->() const
	{
		return Get();
	}

	T& operator*() const
	{
		return *Get();
	}

	bool operator==(T* other) const
	{
		return Get() == other;
	}

	bool operator==(GCPointer<T>& other) const
	{
		return Get() == other.Get();
	}

	bool operator!=(T* other) const
	{
		return Get() != other;
	}

	bool operator!=(GCPointer<T>& other) const
	{
		return Get() != other.Get();
	}
};

#define InitGCPointer(gcp) (gcp._Initialize(dynamic_cast<GCObject*>(this)))

#endif
