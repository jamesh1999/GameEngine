#ifndef __GEOMETRY_INCLUDED__
#define __GEOMETRY_INCLUDED__

#include "Mesh.h"

namespace Geometry
{
	void GenerateCuboid(GameEngine::Resources::Mesh*, float, float, float);
	void GenerateSphere(GameEngine::Resources::Mesh*, float, int, int);
	void GenerateCylinder(GameEngine::Resources::Mesh*, float, float, float, int, int);
	void GenerateQuad(GameEngine::Resources::Mesh*, float, float, int, int);
}

#endif
