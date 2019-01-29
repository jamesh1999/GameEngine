#ifndef __TEXTURE_ARRAY_INCLUDED__
#define __TEXTURE_ARRAY_INCLUDED__

#include "Resource.h"
#include "Texture.h"
#include "ResourcePtr.h"
#include <vector>

namespace GameEngine
{
	namespace Resources
	{
		class TextureArrayLoader;

		// GPU-side texture array
		class TextureArray : public Resource
		{
			friend class TextureArrayLoader;

		private:
			std::vector<ResourcePtr<Texture>> m_textures;
			bool m_transparent = false;

			//GPU resource
			bool m_pushed = false;
			ID3D11Texture2D* m_texture;
			ID3D11ShaderResourceView* m_SRV;

		protected:

			TextureArray* CloneResource() override;

		public:

			TextureArray() = default;

			~TextureArray();
			TextureArray(const TextureArray& other) = delete;
			TextureArray& operator=(const TextureArray& other) = delete;

			TextureArray* Add(Texture*);
			Texture* operator[](int);

			void Push();
			ID3D11ShaderResourceView* GetSRV();

			bool GetTransparent() { return m_transparent; }
			int Find(Texture*);
			int Size() { return m_textures.size(); }
		};
	}
}

#endif
