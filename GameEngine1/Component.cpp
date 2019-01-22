#include "Component.h"
#include "CompositeObject.h"

using namespace GameEngine::Elements;

void Component::Destroy()
{
	auto it = std::find(obj->m_components.begin(), obj->m_components.end(), this);
	obj->m_components.erase(it);
	delete this;
}