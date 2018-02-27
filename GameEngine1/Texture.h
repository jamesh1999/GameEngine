#ifndef __TEXTURE_INCLUDED__
#define __TEXTURE_INCLUDED__

#include <DirectXMath.h>

namespace GameEngine
{
	namespace Resources
	{
		class TextureLoader;
	}
}

namespace Texture
{
	class Texture
	{
	friend class GameEngine::Resources::TextureLoader;

	public :
		float* data;
		int w;
		int h;


	};

	void GaussianBlur(DirectX::XMFLOAT3**, int, int, float);
}

#endif