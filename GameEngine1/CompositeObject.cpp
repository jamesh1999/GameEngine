#include "CompositeObject.h"

using namespace GameEngine::Elements;

// Add to the active world & attach Transform
void CompositeObject::Create()
{
	engine->world->objects.insert(this);

	m_transform = engine->elementFactory->Create<Transform>();
	m_transform->obj = this;
	m_transform->Create();
}

// Passes the Update() onto all attached components
void CompositeObject::Update()
{
	for (const auto& c : m_components)
	{
		if (dynamic_cast<Script*>(c.Get()) == nullptr) continue;
		dynamic_cast<Script*>(c.Get())->Update();
	}
}

// Destroys attached components, removes references & chains Destroy() call
void CompositeObject::Destroy()
{
	for (const auto& c : m_components)
		if (c != nullptr)
			c->Destroy();
	if (m_transform != nullptr) m_transform->Destroy();

	engine->world->objects.erase(this);

	Element::Destroy();
}

CompositeObject& CompositeObject::operator<<(std::istream& in)
{
	*m_transform << in;
	int componentCount;
	in >> componentCount;

	for (int i = 0; i < componentCount; ++i)
	{
		Component* c = static_cast<Component*>(engine->elementFactory->Deserialize(in));
		AttachComponent<Component>(c);
	}

	return *this;
}

CompositeObject& CompositeObject::operator>>(std::ostream& out)
{
	*m_transform >> out;
	out << m_components.size() << '\n';

	for (ElementPtr<Component>& c : m_components)
		*c >> out;

	return *this;
}

// --- Specializations for Transform ---
template <>
Transform* CompositeObject::GetComponent<Transform>()
{
	return m_transform.Get();
}

template <>
Transform* CompositeObject::AttachComponent<Transform>()
{
	exit(-1);
}

template <>
Transform* CompositeObject::AttachComponent<Transform>(Transform* component)
{
	exit(-1);
}