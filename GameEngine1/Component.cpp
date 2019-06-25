#include "Component.h"
#include "CompositeObject.h"

using namespace GameEngine::Elements;

Component& Component::operator>>(std::ostream& out)
{
	// Output the component type only
	out << typeid(*this).name() << '\n';
	return *this;
}

Component& Component::operator<<(std::istream& in)
{
	// Read in the component type only
	std::string type;
	std::getline(in, type);
	return *this;
}

void Component::Destroy()
{
	// Detach from parent CO
	auto it = find(obj->m_components.begin(), obj->m_components.end(), this);
	assert(it != obj->m_components.end());
	obj->m_components.erase(it);

	// Must set dead after
	Element::Destroy();
}
