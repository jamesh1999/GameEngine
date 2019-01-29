#include "World.h"
#include "CompositeObject.h"

using namespace GameEngine::Elements;

void World::Update()
{
	for (CompositeObject* c : objects)
		c->Update();
}
