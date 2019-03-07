#ifndef __LIGHT_INCLUDED__
#define __LIGHT_INCLUDED__

#include <d3d11.h>
#include <DirectXMath.h>
#include "Component.h"

namespace GameEngine
{
	namespace Graphics
	{
		typedef struct
		{
			DirectX::XMFLOAT3 position;
			int type;
			DirectX::XMFLOAT3 colour;
			float radius;
			int shadowTex;
			int padding[3];
		} LightBuffer;

		class Light : public Elements::Component
		{
		public:
			enum Type { POINT = 0, SPOT = 1, DIRECTIONAL = 2, AMBIENT = 3 };

		private:
			ID3D11Texture2D* depthTex;
			ID3D11RenderTargetView* renderTarget;
			ID3D11ShaderResourceView* shaderResource;
			ID3D11Texture2D* depthBuffer;

		public:
			Type m_type = POINT;
			bool m_shadows = false;
			float m_radius = 0.0f;
			DirectX::XMFLOAT3 m_colour;

			ID3D11DepthStencilView* depthBuff;
			D3D11_VIEWPORT vp;
			ID3D11SamplerState* ss;

			~Light();

			void Create() override;
			void FillLightBuffer(LightBuffer*) const;

			ID3D11RenderTargetView* GetRTV() const;
			ID3D11ShaderResourceView* GetSRV() const;
		};
	}
}

#endif
