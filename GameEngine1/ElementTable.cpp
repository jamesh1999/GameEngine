#include "ElementTable.h"

using namespace GameEngine::Elements;

ElementTable::ElementTable()
{
	// Reserve 0 as nullptr flag
	m_elements[0] = nullptr;
}

Element* ElementTable::operator[](Element::UID uid)
{
	if (m_elements.find(uid) == m_elements.end()) return nullptr;
	return m_elements[uid];
}