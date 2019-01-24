#include "Element.h"
#include "ElementPtr.h"

using namespace GameEngine::Elements;

Element::~Element()
{
	// Clear all references to this Element
	for (auto ptr : m_ptrs)
		ptr->Clear();
}