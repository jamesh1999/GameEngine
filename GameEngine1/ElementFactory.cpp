#include "ElementFactory.h"
#include <random>
#include "CompositeObject.h"
#include "ElementTable.h"

using namespace GameEngine::Elements;
using namespace GameEngine;

std::unordered_map<std::string, Element* (*)()> ElementFactory::elements;

ElementFactory::ElementFactory(Engine* e) : engine(e)
{
	// Seed pseudo random generator
	re = std::default_random_engine(rd());
}

void ElementFactory::Initialise(Element* e)
{
	e->engine = engine; // Create engine reference
	e->m_uid = FindFreeUID(); // Assign a unique ID
	engine->elements->m_elements[e->m_uid] = e; // Add to the ElementTable
}

Element::UID ElementFactory::FindFreeUID()
{
	// Poll random UIDs until a free one is located
	Element::UID free;

	while (engine->elements->operator[](free = rDist(re)) != nullptr);
	return free;
}

Element* ElementFactory::Deserialize(std::istream& in)
{
	std::string type;
	in >> type;
	
	Element* c = elements[type]();
	*c << in;

	return c;
}

template <>
CompositeObject* ElementFactory::Create<CompositeObject>()
{
	CompositeObject* obj = new CompositeObject;
	Initialise(obj);
	obj->Create();
	return obj;
}