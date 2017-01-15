#ifndef __OBJECT_MANAGER_INCLUDED__
#define __OBJECT_MANAGER_INCLUDED__

#include <list>

class CompositeObject;

class ObjectManager
{
	friend class CompositeObject;

private:
	static ObjectManager* instance;

	std::list<CompositeObject*> objects;

	//For CompositeObject only:
	static void AddObject(CompositeObject*);
	static void RemoveObject(CompositeObject*);

public:
	ObjectManager();

	static void Update();
};

#endif