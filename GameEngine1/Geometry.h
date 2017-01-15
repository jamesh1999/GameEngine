#ifndef __GEOMETRY_INCLUDED__
#define __GEOMETRY_INCLUDED__

#include "MeshData.h"

namespace Geometry
{
	void GenerateCuboid(SubMesh*, float, float, float);
	void GenerateSphere(SubMesh*, float, int, int);
	void GenerateCylinder(SubMesh*, float, float, float, int, int);
	void GenerateQuad(SubMesh*, float, float, int, int);
}

#endif
