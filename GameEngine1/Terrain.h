#ifndef __TERRAIN_INCLUDED__
#define __TERRAIN_INCLUDED__

#include "MeshData.h"
#include "Perlin.h"
#include "DirectXMath.h"

namespace Terrain
{
	void ApplyHeightmap(SubMesh*);
	void GenerateGroundTexture(DirectX::XMFLOAT3**, int, int, SubMesh*);
	void ThermalErosion(float*, int, int, int, float);
}

#endif