#ifndef __TEXTURE_INCLUDED__
#define __TEXTURE_INCLUDED__

#include <DirectXMath.h>
#include "Resource.h"

namespace GameEngine
{
	namespace Resources
	{
		class TextureLoader;
		class TextureArray;

		// CPU-side individual texture
		class Texture : public Resource
		{
			friend class TextureLoader;
			friend class TextureArray;

		private:

			//Image data
			float* m_data;
			int m_w;
			int m_h;
			bool m_opaque = true;

		protected:

			Texture* CloneResource() override;

		public:

			~Texture();

			int GetWidth() const;
			int GetHeight() const;
			bool IsOpaque() const;

			Texture* Resize(int, int);
		};
	}
}

namespace //Texture
{
	void GaussianBlur(DirectX::XMFLOAT3**, int, int, float);
}

#endif
