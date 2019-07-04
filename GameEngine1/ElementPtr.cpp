#include "ElementPtr.h"
#include "CompositeObject.h"

using namespace GameEngine::Elements;

template <>
CompositeObject *const ElementPtr<CompositeObject>::Get() const
{
	if (m_engine == nullptr) return nullptr;
	return static_cast<CompositeObject*>(m_engine->elements->operator[](m_uid));
}