#ifndef __STATIC_BATCHER_INCLUDED__
#define __STATIC_BATCHER_INCLUDED__

#include <vector>
#include "CompositeObject.h"
#include "Engine.h"
#include "Renderer.h"

class StaticBatcher
{
private:
	static void AggregateRenderers(GameEngine::Elements::CompositeObject*, std::vector<GameEngine::Renderer*>&);
public:
	static void BatchFrom(GameEngine::Elements::CompositeObject*, GameEngine::Engine*);
	static void CullHeirarchy(GameEngine::Elements::CompositeObject*);
};

#endif
