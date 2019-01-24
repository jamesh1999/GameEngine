#include "GarbageCollector.h"
#include "Component.h"

GCSettings GarbageCollector::settings;
std::vector<GCObject*> GarbageCollector::objects;
std::vector<GCObject*> GarbageCollector::base;
int GarbageCollector::curID = 0;

void GarbageCollector::recurseObjectTree(GCObject* obj)
{
	obj->flags = GCObject::WHITE;
	
	for (GCPointer<GCObject>* child : obj->childGCPs)
		recurseObjectTree(child->Get());
}

void GarbageCollector::RegisterBaseObject(GCObject* obj)
{
	base.push_back(obj);
}

void GarbageCollector::RemoveBaseObject(GCObject* obj)
{
	auto it = std::find(base.begin(), base.end(), obj);
	base.erase(it);
}

void GarbageCollector::RegisterObject(GCObject* obj)
{
	objects.push_back(obj);
	obj->id = curID++;
}

void GarbageCollector::RunCollector()
{
	for (GCObject* obj : objects)
		if (obj->flags != GCObject::NO_GC)
			obj->flags = GCObject::BLACK;

	for (GCObject* obj : base)
		obj->flags = GCObject::GREY;

	bool done = false;
	while (!done)
	{
		done = true;
		for (GCObject* obj : objects)
			if (obj->flags == GCObject::GREY)
			{
				done = false;
				for (GCPointer<GCObject>* child : obj->childGCPs)
					if (child->Get()->flags == GCObject::BLACK)
					child->Get()->flags = GCObject::GREY;
				obj->flags = GCObject::WHITE;
			}
	}

	for (GCObject* obj : objects)
		if (obj->flags == GCObject::BLACK && obj->m_alive)
			obj->Destroy();

	for (GCObject* obj : objects)
		for (GCPointer<GCObject>* gcp : obj->childGCPs)
			gcp->Get();

	std::vector<GCObject*> nobjects;
	for (GCObject* obj : objects)
		if (obj->m_alive)
			nobjects.push_back(obj);
		else if (dynamic_cast<GameEngine::Elements::Component*>(obj) != nullptr)
			delete dynamic_cast<GameEngine::Elements::Component*>(obj);
		else
			delete obj;
	objects = nobjects;
}

void GarbageCollector::RefreshTracked()
{

}

void GarbageCollector::UpdateSettings(GCSettings* nSettings)
{
	settings = *nSettings;
}

const GCSettings* GarbageCollector::GetSettings()
{
	return &settings;
}
