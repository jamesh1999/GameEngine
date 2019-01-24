#ifndef __GCOBJECT_INCLUDED__
#define __GCOBJECT_INCLUDED__
#include <list>

template <class T>
class GCPointer;

class GarbageCollector;

class GCObject
{
	friend class GarbageCollector;
	template <class T> friend class GCPointer;
	
private:
	enum Flags {BLACK = 0, GREY = 1, WHITE = 2, NO_GC = 3};
	char flags : 2;
	unsigned id : 30;

	bool m_alive = true;

	std::list<GCPointer<GCObject>*> childGCPs;

public:
	GCObject();

	virtual void Destroy();
};

#endif