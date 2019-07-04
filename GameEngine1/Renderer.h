#ifndef __RENDERER_INCLUDED___
#define __RENDERER_INCLUDED___

#include "Component.h"
#include "Material.h"
#include "Mesh.h"
#include "ResourcePtr.h"
#include "TextureArray.h"

namespace GameEngine
{
	class Renderer : public Elements::Component
	{
		//friend class GraphicsController;

	public:
		Resources::ResourcePtr<Material> mat;
		Resources::ResourcePtr<Resources::Mesh> mesh;
		Resources::ResourcePtr<Resources::TextureArray> m_textures;
		bool m_active = true;
		bool m_init = false;

	public:

		void Destroy() override;

		void Render(int);

		void Init(Material* mat, Resources::Mesh* mesh);

		void SetMesh(Resources::Mesh*);
		void SetMaterial(Material*);
		void SetTexture(Resources::TextureArray*);
		void SetTexture(Resources::Texture*);

		bool GetTransparent() const;

		Resources::Mesh* GetMesh() const;
	};
}

#endif
