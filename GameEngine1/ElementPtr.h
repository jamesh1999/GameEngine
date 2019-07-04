#ifndef __ELEMENT_PTR_INCLUDED__
#define __ELEMENT_PTR_INCLUDED__

#include "Element.h"
#include "Engine.h"
#include "ElementTable.h"

namespace GameEngine
{
	namespace Elements
	{
		class CompositeObject;

		// Refers to an element with a pointer or UID & switches to nullptr on element destruction
		template <class TElement>
		class ElementPtr
		{
			template <class>
			friend struct std::hash;

		private:
			Engine* m_engine = nullptr;
			Element::UID m_uid = 0;

		public:

			ElementPtr(Engine* engine, Element::UID uid) : m_engine(engine), m_uid(uid)
			{
				static_assert(std::is_base_of<Element, TElement>::value);
			}
			ElementPtr(TElement* element)
			{
				static_assert(std::is_base_of<Element, TElement>::value);
				if (element == nullptr) return;

				m_engine = element->GetEngine();
				m_uid = element->GetUID();
			}
			ElementPtr() : ElementPtr(nullptr) {}

			TElement *const Get() const;

			TElement* operator->() const
			{
				return Get();
			}

			TElement& operator*() const
			{
				return *Get();
			}

			bool operator==(const TElement* other) const
			{
				return Get() == other;
			}

			bool operator==(const ElementPtr<TElement>& other) const
			{
				return Get() == other.Get();
			}

			bool operator!=(const TElement* other) const
			{
				return Get() != other;
			}

			bool operator!=(const ElementPtr<TElement>& other) const
			{
				return Get() != other.Get();
			}
		};

		template <class TElement>
		TElement *const ElementPtr<TElement>::Get() const
		{
			if (m_engine == nullptr) return nullptr;
			return static_cast<TElement*>(m_engine->elements->operator[](m_uid));
		}

		template <>
		CompositeObject *const ElementPtr<CompositeObject>::Get() const;
	}
}

namespace std
{
	template <class TElement>
	struct hash<GameEngine::Elements::ElementPtr<TElement>>
	{
		size_t operator()(const GameEngine::Elements::ElementPtr<TElement>& ptr) const
		{
			// Just hash on element pointer
			return hash<Element::UID>()(ptr.m_uid);
		}
	};
}

#endif
