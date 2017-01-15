#ifndef __SCRIPT_INCLUDED__
#define __SCRIPT_INCLUDED__

#include "Component.h"

class Script : public Component
{
public:
	virtual void Update() = 0;
	virtual void Create() = 0;
};

#endif