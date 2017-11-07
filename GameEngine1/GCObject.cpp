#include "GCObject.h"
#include "GarbageCollector.h"

GCObject::GCObject()
{
	GarbageCollector::RegisterObject(this);
}
