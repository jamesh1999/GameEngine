#include "ObjectManager.h"
#include <iostream>
#include "CompositeObject.h"
#include "World.h"

ObjectManager* ObjectManager::instance = nullptr;

ObjectManager::ObjectManager(GameEngine::Engine* e)
{
	if(instance != nullptr)
	{
		std::cerr << "ObjectManager: Multiple instances created!" << std::endl;
		throw std::exception("ObjectManager: multiple instances");
	}
	else
		instance = this;

	engine = e;
}