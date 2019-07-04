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