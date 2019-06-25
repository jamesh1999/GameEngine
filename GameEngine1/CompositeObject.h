#ifndef __COMPOSITE_OBJECT_INCLUDED__
#define __COMPOSITE_OBJECT_INCLUDED__

#include <vector>
#include "Component.h"
#include "Element.h"
#include "ElementPtr.h"
#include "Script.h"
#include "Transform.h"
#include "World.h"
#include "ElementFactory.h"

namespace GameEngine
{
	namespace Elements
	{
		class CompositeObject : public Element
		{
		public:
			std::vector<ElementPtr<Component>> m_components;
			ElementPtr<Transform> m_transform;

		public:

			void Update()
			{
				for (const auto& c : m_components)
				{
					if (dynamic_cast<Script*>(c.Get()) == nullptr) continue;
					dynamic_cast<Script*>(c.Get())->Update();
				}
			}

			void Destroy() override
			{
				for (const auto& c : m_components)
					if (c != nullptr)
						c->Destroy();
				if (m_transform != nullptr) m_transform->Destroy();

				engine->world->objects.erase(this);

				Element::Destroy();
			}

			template <class T>
			T* GetComponent()
			{
				for (size_t i = 0; i < m_components.size(); ++i)
				{
					if (dynamic_cast<T*>(m_components[i].Get()) != nullptr)
						return dynamic_cast<T*>(m_components[i].Get());
				}

				return nullptr;
			}

			template <class T>
			T* AttachComponent()
			{
				T* component = engine->elementFactory->Create<T>();
				m_components.push_back(component);
				static_cast<Component*>(component)->obj = this;
				static_cast<Component*>(component)->Create();
				return component;
			}

			template <class T>
			T* AttachComponent(T* component)
			{
				m_components.push_back(component);
				static_cast<Component*>(component)->obj = this;
				static_cast<Component*>(component)->Create();
				return component;
			}

			CompositeObject& operator<<(std::istream& in)
			{
				*m_transform << in;
				int componentCount;
				in >> componentCount;

				for (int i = 0; i < componentCount; ++i)
				{
					Component* c = engine->elementFactory->Deserialize(in);
					AttachComponent<Component>(c);
				}

				return *this;
			}

			CompositeObject& operator>>(std::ostream& out)
			{
				*m_transform >> out;
				out << m_components.size() << '\n';

				for (ElementPtr<Component>& c : m_components)
					*c >> out;

				return *this;
			}
		};

		template <>
		Transform* CompositeObject::GetComponent<Transform>();

		template <>
		Transform* CompositeObject::AttachComponent<Transform>();

		template <>
		Transform* CompositeObject::AttachComponent<Transform>(Transform*);
	}
}

#endif
