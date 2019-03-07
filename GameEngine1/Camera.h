#ifndef __CAMERA_INCLUDED__
#define __CAMERA_INCLUDED__

#include <d3d11.h>
#include <DirectXMath.h>
#include "Component.h"

namespace GameEngine
{
	namespace Graphics
	{
		class Camera : public Elements::Component
		{
		public:
			struct CameraBuffer
			{
				DirectX::XMFLOAT4X4A worldView;
				DirectX::XMFLOAT4X4A viewProj;
				DirectX::XMFLOAT3A position;
				float padding;
			};

		private:
			D3D11_VIEWPORT vP;
			ID3D11RenderTargetView** m_renderTargets;
			ID3D11DepthStencilView* m_depthBuffer;

		public:
			bool m_active = false;

			void Create() override;
			void Draw();
		};
	}
}

#endif
