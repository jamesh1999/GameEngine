#ifndef __TERRAIN_INCLUDED__
#define __TERRAIN_INCLUDED__

#include <DirectXMath.h>
#include "Mesh.h"

namespace Terrain
{
	void ApplyHeightmap(GameEngine::Resources::Mesh*);
	void GenerateGroundTexture(DirectX::XMFLOAT3**, int, int, GameEngine::Resources::Mesh*);
	void ThermalErosion(float*, int, int, int, float);
}

#endif
