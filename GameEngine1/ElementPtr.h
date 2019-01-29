#ifndef __ELEMENT_PTR_INCLUDED__
#define __ELEMENT_PTR_INCLUDED__

#include "Element.h"

namespace GameEngine
{
	namespace Elements
	{
		class CompositeObject;

		template <class TElement>
		class ElementPtr
		{
			template <class>
			friend struct std::hash;
			friend class Element;

		private:
			TElement* m_element = nullptr;

			void AttachPtr();
			void DetachPtr();

			void Clear() { m_element = nullptr; }

		public:

			ElementPtr(TElement* element)
			{
				static_assert(std::is_base_of<Element, TElement>::value);

				m_element = element;
				AttachPtr();
			}

			ElementPtr() : ElementPtr(nullptr)
			{
			};

			~ElementPtr() { DetachPtr(); }

			ElementPtr(const ElementPtr<TElement>& other)
			{
				m_element = other.m_element;
				AttachPtr();
			}

			ElementPtr<TElement>& operator=(const ElementPtr<TElement>& other)
			{
				DetachPtr();

				m_element = other.m_element;
				AttachPtr();

				return *this;
			}

			ElementPtr<TElement>& operator=(TElement* element)
			{
				DetachPtr();

				m_element = element;
				AttachPtr();

				return *this;
			}

			TElement* Get() const
			{
				return m_element;
			}

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

		// Attach this ptr to the new element
		template <class TElement>
		void ElementPtr<TElement>::DetachPtr()
		{
			if (m_element == nullptr) return;
			static_cast<Element*>(m_element)->m_ptrs.erase(reinterpret_cast<ElementPtr<Element>*>(this));
		}

		template <>
		void ElementPtr<CompositeObject>::DetachPtr();

		// Remove reference to this ptr from element
		template <class TElement>
		void ElementPtr<TElement>::AttachPtr()
		{
			if (m_element == nullptr) return;
			static_cast<Element*>(m_element)->m_ptrs.insert(reinterpret_cast<ElementPtr<Element>*>(this));
		}

		template <>
		void ElementPtr<CompositeObject>::AttachPtr();
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
			return hash<TElement*>()(ptr.m_element);
		}
	};
}

#endif
