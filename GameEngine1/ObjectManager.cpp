#include "ObjectManager.h"
#include "CompositeObject.h"

ObjectManager* ObjectManager::instance = nullptr;

void ObjectManager::AddObject(CompositeObject* obj)
{
	instance->objects.push_back(obj);
}

void ObjectManager::RemoveObject(CompositeObject* obj)
{
	instance->objects.remove(obj);
}

ObjectManager::ObjectManager()
{
	instance = this;
}

void ObjectManager::Update()
{
	for (CompositeObject* obj : instance->objects)
		obj->Update();
}