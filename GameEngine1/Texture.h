#ifndef __TEXTURE_INCLUDED__
#define __TEXTURE_INCLUDED__

#include <DirectXMath.h>

class TextureLoader;

namespace Texture
{
	class Texture
	{
	friend class TextureLoader;

	private:
		float* data;
		int w;
		int h;


	};

	void GaussianBlur(DirectX::XMFLOAT3**, int, int, float);
}

#endif