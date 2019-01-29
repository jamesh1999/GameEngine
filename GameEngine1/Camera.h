#ifndef __CAMERA_INCLUDED__
#define __CAMERA_INCLUDED__

#include "Component.h"

struct Camera : GameEngine::Elements::Component
{
	void Create() override;
};

#endif
