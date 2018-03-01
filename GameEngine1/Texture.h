#ifndef __TEXTURE_INCLUDED__
#define __TEXTURE_INCLUDED__

#include <DirectXMath.h>
#include "Resource.h"
#include <d3d11.h>

namespace GameEngine
{
	namespace Resources
	{
		class TextureLoader;
		class TextureArray;

		class Texture : public Resource
		{
			friend class GameEngine::Resources::TextureLoader;
			friend class GameEngine::Resources::TextureArray;

		private:

			//Image data
			float* m_data;
			int m_w;
			int m_h;

			//GPU resource
			bool m_pushed = false;
			ID3D11Texture2D* m_texture;
			ID3D11ShaderResourceView* m_SRV;

		public:

			int GetWidth() const;
			int GetHeight() const;

			void Resize(int, int);

			void Push();
			ID3D11ShaderResourceView* GetSRV();
		};
	}
}

namespace //Texture
{
	void GaussianBlur(DirectX::XMFLOAT3**, int, int, float);
}

#endif