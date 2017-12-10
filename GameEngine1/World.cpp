#include "World.h"
#include "CompositeObject.h"

using namespace GameEngine::ObjectSystem;

void World::Update()
{
	for (CompositeObject* c : objects)
		c->Update();
}