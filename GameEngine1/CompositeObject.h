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

			void Create() override;
			void Update();
			void Destroy() override;

			template <class T>
			T* GetComponent();
			template <class T>
			T* AttachComponent();
			template <class T>
			T* AttachComponent(T* component);

			CompositeObject& operator<<(std::istream& in);
			CompositeObject& operator>>(std::ostream& out);
		};

		template <class T>
		T* CompositeObject::GetComponent()
		{
			for (size_t i = 0; i < m_components.size(); ++i)
			{
				if (dynamic_cast<T*>(m_components[i].Get()) != nullptr)
					return dynamic_cast<T*>(m_components[i].Get());
			}

			return nullptr;
		}

		template <>
		Transform* CompositeObject::GetComponent<Transform>();

		template <class T>
		T* CompositeObject::AttachComponent()
		{
			T* component = engine->elementFactory->Create<T>();
			return AttachComponent<T>(component);
		}

		template <>
		Transform* CompositeObject::AttachComponent<Transform>();

		template <class T>
		T* CompositeObject::AttachComponent(T* component)
		{
			m_components.push_back(component);
			static_cast<Component*>(component)->obj = this;
			static_cast<Component*>(component)->Create();
			return component;
		}

		template <>
		Transform* CompositeObject::AttachComponent<Transform>(Transform*);
	}
}

#endif
