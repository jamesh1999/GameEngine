#include "Element.h"
#include "ElementPtr.h"
#include "ElementTable.h"

using namespace GameEngine;
using namespace GameEngine::Elements;

Element::~Element()
{
	// Redirect the ID to nullptr
	m_engine->elements->m_elements[m_uid] = nullptr;
}

void Element::Destroy()
{
	delete this;
}

Engine* Element::GetEngine()
{
	return m_engine;
}

Element::UID Element::GetUID()
{
	return m_uid;
}

// Read nothing this is the job of the ElementFactory
Element& Element::operator<<(std::istream& in) { return *this; }

Element& Element::operator>>(std::ostream& out)
{
	out << typeid(*this).name() << '\n';
	out << m_uid << '\n';
	return *this;
}