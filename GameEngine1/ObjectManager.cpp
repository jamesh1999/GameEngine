#include "ObjectManager.h"
#include <iostream>
#include "CompositeObject.h"

ObjectManager* ObjectManager::instance = nullptr;

ObjectManager::ObjectManager()
{
	if(instance != nullptr)
	{
		std::cerr << "ObjectManager: Multiple instances created!" << std::endl;
		throw std::exception("ObjectManager: multiple instances");
	}
	else
		instance = this;
}

void ObjectManager::Update()
{
	for (CompositeObject* obj : instance->objects)
		obj->Update();
}

CompositeObject* ObjectManager::CreateObject()
{
	CompositeObject* obj = new CompositeObject;
	instance->objects.push_back(obj);

	return obj;
}

void ObjectManager::FreeObject(CompositeObject* obj)
{
	instance->objects.remove(obj);
	for (Component* c : obj->m_components)
		FreeComponent(c);
	delete obj;
}
