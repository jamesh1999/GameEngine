#ifndef __TEXTURE_ARRAY_INCLUDED__
#define __TEXTURE_ARRAY_INCLUDED__

#include "Resource.h"
#include "Texture.h"
#include <vector>

namespace GameEngine
{
	namespace Resources
	{
		class TextureArray : public Resource
		{
		private:
			std::vector<Texture*> m_textures;

			//GPU resource
			bool m_pushed = false;
			ID3D11Texture2D* m_texture;
			ID3D11ShaderResourceView* m_SRV;

		public:

			void Add(Texture*);
			Texture* operator[](int);

			void Push();
			ID3D11ShaderResourceView* GetSRV();
		};
	}
}

#endif