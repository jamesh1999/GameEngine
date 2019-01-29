#include "ElementPtr.h"
#include "CompositeObject.h"

using namespace GameEngine::Elements;

// Attach this ptr to the new element
template <>
void ElementPtr<CompositeObject>::DetachPtr()
{
	if (m_element == nullptr) return;
	static_cast<Element*>(m_element)->m_ptrs.erase(reinterpret_cast<ElementPtr<Element>*>(this));
}

// Remove reference to this ptr from element
template <>
void ElementPtr<CompositeObject>::AttachPtr()
{
	if (m_element == nullptr) return;
	static_cast<Element*>(m_element)->m_ptrs.insert(reinterpret_cast<ElementPtr<Element>*>(this));
}
