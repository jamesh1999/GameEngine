#include "ElementFactory.h"
#include <random>
#include "CompositeObject.h"
#include "ElementTable.h"

using namespace GameEngine;
using namespace GameEngine::Elements;

std::unordered_map<std::string, Element* (*)()> ElementFactory::elements;

ElementFactory::ElementFactory(Engine* e) : m_engine(e)
{
	// Seed pseudo random generator
	re = std::default_random_engine(rd());
}

Element* ElementFactory::DeserialiseHeader(std::istream& in)
{
	std::string type;
	std::getline(in, type);
	Element::UID uid;
	in >> uid;

	Element* e = elements[type]();
	Initialise(e, uid);
}

Engine* ElementFactory::GetEngine()
{
	return m_engine;
}

void ElementFactory::_RegisterElement(std::string name, Element* (*instantiator)())
{
	elements[name] = instantiator;
}

void ElementFactory::Initialise(Element* e, Element::UID uid)
{
	e->m_engine = GetEngine(); // Create engine reference
	e->m_uid = uid; // Assign a unique ID
	GetEngine()->elements->m_elements[e->m_uid] = e; // Add to the ElementTable
}

Element::UID ElementFactory::FindFreeUID()
{
	// Poll random UIDs until a free one is located
	Element::UID free;

	while (GetEngine()->elements->operator[](free = rDist(re)) != nullptr);
	return free;
}

Element* ElementFactory::Deserialize(std::istream& in)
{
	// Read element header then continue as normal
	Element* e = DeserialiseHeader(in);

	*e << in;
	return e;
}

template <>
CompositeObject* ElementFactory::Create<CompositeObject>()
{
	CompositeObject* obj = new CompositeObject;
	Initialise(obj, FindFreeUID());
	obj->Create();
	return obj;
}