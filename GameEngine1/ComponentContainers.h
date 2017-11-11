#ifndef __COMPONENT_CONTAINERS_INCLUDED__
#define __COMPONENT_CONTAINERS_INCLUDED__

#include <unordered_set>
#include "Component.h"

struct Hash {
	size_t operator()(const Component* c) const
	{
		return c->GetID();
	}
};

struct Pred {
	bool operator()(const Component* a, const Component* b) const
	{
		return a->GetID() == b->GetID();
	}
};

template<class T>
using ComponentSet = std::unordered_set<T, Hash, Pred>;

#endif