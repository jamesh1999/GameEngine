#include "World.h"
#include "CompositeObject.h"
#include "ElementFactory.h"

using namespace GameEngine;
using namespace GameEngine::Elements;

World::World(Engine* e) : engine(e) {}

void World::Update()
{
	for (CompositeObject* c : objects)
		c->Update();
}

World& World::operator<<(std::istream& in)
{
	int num;
	in >> num;
	objects.reserve(num);

	for (int i = 0; i < num; ++i)
		objects.insert(static_cast<CompositeObject*>(engine->elementFactory->Deserialize(in)));

	return *this;
}

World& World::operator>>(std::ostream& out)
{
	out << objects.size() << '\n';
	for (CompositeObject* c : objects)
	{
		*c >> out;
		out << '\n';
	}

	return *this;
}