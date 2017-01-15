#ifndef __COMPOSITE_OBJECT_INCLUDED__
#define __COMPOSITE_OBJECT_INCLUDED__

#include <vector>
#include "ObjectManager.h"
#include "Component.h"
#include "Transform.h"
#include "Script.h"

class CompositeObject
{
private:
	std::vector<Component*> m_components;
	Transform* m_transform;

public:
	CompositeObject() 
	{
		ObjectManager::AddObject(this);
		m_transform = new Transform;
	}

	~CompositeObject()
	{
		ObjectManager::RemoveObject(this);

		//Cleanup
		for (Component* c : m_components)
			delete c;

		delete m_transform;
	}

	void Update()
	{
		for (int i = 0; i < m_components.size(); ++i)
		{
			if (dynamic_cast<Script*>(m_components[i]) != nullptr)
				dynamic_cast<Script*>(m_components[i])->Update();
		}
	}

	template <class T>
	T* GetComponent()
	{
		for (int i = 0; i < m_components.size(); ++i)
		{
			if (dynamic_cast<T*>(m_components[i]) != nullptr)
				return dynamic_cast<T*>(m_components[i]);
		}

		return nullptr;
	}
	template <>
	Transform* GetComponent<Transform>()
	{
		return m_transform;
	}

	template<class T>
	T* AttachComponent()
	{
		T* component = new T;
		m_components.push_back(component);
		reinterpret_cast<Component*>(component)->obj = this;
		if (dynamic_cast<Script*>(component) != nullptr)
			dynamic_cast<Script*>(component)->Create();
		return component;
	}
	template<>
	Transform* AttachComponent<Transform>()
	{
		exit(-1);
	}
};

#endif