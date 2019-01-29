#ifndef __PARTICLE_SYSTEM_INCLUDED__
#define __PARTICLE_SYSTEM_INCLUDED__

#include <DirectXMath.h>
#include <d3d11.h>
#include <vector>
#include "Engine.h"
#include "Mesh.h"
#include "ResourcePtr.h"
#include "TextureArray.h"

namespace GameEngine
{
	namespace Graphics
	{
		class ParticleSystem
		{
		private:

			ID3D11VertexShader* vtx;
			ID3D11PixelShader* pix;

			std::vector<Vertex> vertices;
			std::vector<unsigned> indices;

			ID3D11Buffer* vertexBuffer = nullptr;
			ID3D11Buffer* indexBuffer = nullptr;

			Resources::ResourcePtr<Resources::TextureArray> m_texture;

			ID3D11BlendState* blendState;
			ID3D11DepthStencilState* depthState;

			std::vector<DirectX::XMFLOAT3> position;
			std::vector<DirectX::XMFLOAT3> velocity;
			float lifetime = 0.0f;
		public:
			Engine* engine = nullptr;

			ParticleSystem(Engine*);

			void Initialise(DirectX::XMFLOAT3);
			void Update();

			void Draw();
		};
	}
}

#endif
