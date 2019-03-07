#include "ElementFactory.h"

using namespace GameEngine::Elements;
using namespace GameEngine;

int ElementFactory::id = 0;

ElementFactory::ElementFactory(Engine* e) : engine(e) {}

template <>
CompositeObject* ElementFactory::Create<CompositeObject>()
{
	//static_assert(std::is_base_of<Element, T>::value);

	CompositeObject* obj = new CompositeObject;
	obj->engine = engine;

	//For CompositeObjects assign a unique ID & add them to the active World
	engine->world->objects.insert(obj);
	Transform* t = Create<Transform>();
	obj->m_transform = t;
	t->obj = obj;
	t->Create();

	return obj;
}
