#include "ElementTable.h"

using namespace GameEngine::Elements;

Element* ElementTable::operator[](Element::UID uid)
{
	if (m_elements.find(uid) == m_elements.end()) return nullptr;
	return m_elements[uid];
}