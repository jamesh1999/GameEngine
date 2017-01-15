#ifndef __TEXTURE_INCLUDED__
#define __TEXTURE_INCLUDED__

#include <DirectXMath.h>

namespace Texture
{
	void GaussianBlur(DirectX::XMFLOAT3**, int, int, float);
}

#endif