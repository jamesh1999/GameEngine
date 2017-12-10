#ifndef __CAMERA_INCLUDED__
#define __CAMERA_INCLUDED__

#include <DirectXMath.h>
#include "Component.h"

struct Camera : Component
{
	void Create() override;
};

#endif