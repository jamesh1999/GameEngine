#ifndef __GRAPHICS_OBJECT_INCLUDED___
#define __GRAPHICS_OBJECT_INCLUDED___

#include <DirectXMath.h>
#include <vector>
#include "Component.h"
#include "Material.h"
#include "Mesh.h"
#include "ResourceRef.h"
#include "TextureArray.h"

namespace GameEngine
{
	class Renderer : public Elements::Component
	{
		//friend class GraphicsController;

	public:
		Resources::ResourceRef<Material> mat;
		Resources::Mesh* mesh;
		Resources::ResourceRef<Resources::TextureArray> m_textures;
		bool m_active = true;

	public:


		void Render(int);

		void Init(Material* mat, Resources::Mesh* mesh);

		void SetMesh(Resources::Mesh*);
		void SetMaterial(Material*);
		void SetTexture(Resources::TextureArray*);
		void SetTexture(Resources::Texture*);

		bool GetTransparent();

		Resources::Mesh* GetMesh();
	};
}

#endif