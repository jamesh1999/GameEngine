#include "GCObject.h"
#include "GarbageCollector.h"

GCObject::GCObject()
{
	GarbageCollector::RegisterObject(this);
}

void GCObject::Destroy()
{
	m_alive = false;
}