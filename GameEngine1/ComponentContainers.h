#ifndef __COMPONENT_CONTAINERS_INCLUDED__
#define __COMPONENT_CONTAINERS_INCLUDED__

#include <unordered_set>
#include "Component.h"

struct Hash {
	size_t operator()(const GameEngine::ObjectSystem::Component* c) const
	{
		return c->GetID();
	}
};

struct Pred {
	bool operator()(const GameEngine::ObjectSystem::Component* a, const GameEngine::ObjectSystem::Component* b) const
	{
		return a->GetID() == b->GetID();
	}
};

template<class T>
using ComponentSet = std::unordered_set<T, Hash, Pred>;

#endif