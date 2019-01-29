#include "Component.h"
#include "CompositeObject.h"

using namespace GameEngine::Elements;

void Component::Destroy()
{
	// Detach from parent CO
	auto it = find(obj->m_components.begin(), obj->m_components.end(), this);
	assert(it != obj->m_components.end());
	obj->m_components.erase(it);

	// Must set dead after
	Element::Destroy();
}
