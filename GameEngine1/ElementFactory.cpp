#include "ElementFactory.h"

using namespace GameEngine::ObjectSystem;

int ElementFactory::id = 0;

template <>
CompositeObject* ElementFactory::Create<CompositeObject>()
{
	//static_assert(std::is_base_of<Element, T>::value);

	CompositeObject* obj = new CompositeObject;
	obj->engine = engine;

	//For CompositeObjects assign a unique ID & add them to the active World
	engine->world->objects.push_back(obj);
	Transform* t = Create<Transform>();
	obj->m_transform = t;

	return obj;
}