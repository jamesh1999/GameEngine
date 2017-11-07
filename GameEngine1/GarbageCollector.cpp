#include "GarbageCollector.h"

GCSettings GarbageCollector::settings;
std::vector<GCObject*> GarbageCollector::objects;
std::vector<GCObject*> GarbageCollector::tracked;
int GarbageCollector::curID = 0;

void GarbageCollector::recurseObjectTree(GCObject* obj)
{
	obj->flags = GCObject::WHITE;
	
	for (GCPointer<void>* child : obj->childGCPs)
		recurseObjectTree(static_cast<GCObject*>(child->m_object));
}

void GarbageCollector::RegisterBaseObject(GCObject* obj)
{
	objects.push_back(obj);
}

void GarbageCollector::RegisterObject(GCObject* obj)
{
	//objects.push_back(obj);
	obj->id = curID++;
	if (obj->flags != GCObject::NO_GC)
		tracked.push_back(obj);
}

void GarbageCollector::RunCollector()
{
	for (GCObject* obj : tracked)
		obj->flags = GCObject::BLACK;

	for (GCObject* obj : objects)
		recurseObjectTree(obj);

	for (auto it = tracked.begin(); it != tracked.end(); ++it)
		if ((*it)->flags == GCObject::BLACK)
		{
			delete *it;
			*it = nullptr;
		}

	std::vector<GCObject*> ntracked;
	for (GCObject* obj : tracked)
		if (obj != nullptr)
			ntracked.push_back(obj);
	tracked = ntracked;
}

void GarbageCollector::RefreshTracked()
{
	tracked.clear();
	for (GCObject* obj : objects)
		if (obj->flags != GCObject::NO_GC)
			tracked.push_back(obj);
}

void GarbageCollector::UpdateSettings(GCSettings* nSettings)
{
	settings = *nSettings;
}

const GCSettings* GarbageCollector::GetSettings()
{
	return &settings;
}
