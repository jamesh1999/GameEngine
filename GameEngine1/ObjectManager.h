#ifndef __OBJECT_MANAGER_INCLUDED__
#define __OBJECT_MANAGER_INCLUDED__

#include <list>
#include <vector>
#include "Component.h"

class CompositeObject;

class ObjectManager
{
private:
	static ObjectManager* instance;

	short componentCnt = 0;
	std::vector<short> componentCnts;

	std::list<CompositeObject*> objects;

public:
	ObjectManager();

	static void Update();

	template<class T>
	static short GetTypeID();

	static CompositeObject* CreateObject();
	static void FreeObject(CompositeObject*);
	template<class T>
	static T* CreateComponent();
	template<class T>
	static void FreeComponent(T*);
};

template <class T>
short ObjectManager::GetTypeID()
{
	//Create ID for component type
	static short typeID = instance->componentCnt++;

	//Add count tracker to vector
	if (typeID == instance->componentCnts.size())
		instance->componentCnts.push_back(0);

	return typeID;
}

//Create a new component and assign an ID
template <class T>
T* ObjectManager::CreateComponent()
{
	short typeID = GetTypeID<T>();
	short individualID = instance->componentCnts[typeID]++;
	unsigned fullID = (typeID << 16) + individualID;

	T* comp = new T;
	dynamic_cast<Component*>(comp)->id = fullID;

	return comp;
}

template <class T>
void ObjectManager::FreeComponent(T* component)
{
	delete component;
}

#endif
