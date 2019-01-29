#include "CompositeObject.h"

using namespace GameEngine::Elements;

template <>
Transform* CompositeObject::GetComponent<Transform>()
{
	return m_transform.Get();
}

template<>
Transform* CompositeObject::AttachComponent<Transform>()
{
	exit(-1);
}

template<>
Transform* CompositeObject::AttachComponent<Transform>(Transform* component)
{
	exit(-1);
}