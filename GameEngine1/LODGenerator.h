/*#ifndef __LOD_GENERATOR_INCLUDED__
#define __LOD_GENERATOR_INCLUDED__

#include <vector>
#include "MeshData.h"

class LODGenerator
{
private:

	float GetVolume(SubMesh*, int);
	

public:
	SubMesh Generate(SubMesh*, float);
};

#endif*/