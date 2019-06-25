#ifndef __SCRIPT_INCLUDED__
#define __SCRIPT_INCLUDED__

#include "Component.h"

class Script : public GameEngine::Elements::Component
{
public:
	virtual void Update() { }

	Script& operator>>(std::ostream&) { return *this; }
	Script& operator<<(std::istream&) { return *this; }
};

#endif
