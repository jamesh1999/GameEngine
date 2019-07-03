#include "Element.h"
#include "ElementPtr.h"
#include "ElementTable.h"

using namespace GameEngine;
using namespace GameEngine::Elements;

Element::~Element()
{
	// Clear all references to this Element
	for (auto ptr : m_ptrs)
		ptr->Clear();

	// Free the ID by removing from table
	engine->elements->m_elements.erase(GetUID());
}

void Element::Destroy()
{
	delete this;
}

Element::UID Element::GetUID()
{
	return m_uid;
}