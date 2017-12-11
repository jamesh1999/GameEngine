#ifndef __SCRIPT_INCLUDED__
#define __SCRIPT_INCLUDED__

#include "Component.h"

class Script : public GameEngine::ObjectSystem::Component
{
public:
	virtual void Update() {};
};

#endif