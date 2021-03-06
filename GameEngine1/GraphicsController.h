#ifndef __GRAPHICS_CONTROLLER_INCLUDED__
#define __GRAPHICS_CONTROLLER_INCLUDED__

#include <d3d11.h>
#include <vector>
#include <windows.h>
#include "Renderer.h"
#include "Camera.h"
#include "Light.h"
#include "Window.h"
#include "GeometryBufferContainer.h"
#include "RenderQueue.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace GameEngine
{
	namespace Graphics
	{
		enum SHADER_TYPE
		{
			Shader_Type_Vertex,
			Shader_Type_Pixel
		};

		class GraphicsController
		{
			ID3D11Buffer* cBufferFrame;
			ID3D11Buffer* cBufferObject;
			ID3D11Buffer* cBufferLight;


			//Utils::PriorityQueue<GameEngine::Renderer*> renderers;
		public:
			RenderQueue rq;
		private:
			UINT numQualityLevels;
		private:
			Light* m_light;
			std::vector<Light*> m_lights;
			D3D11_VIEWPORT vP;

		public:
			Engine* engine;

			Camera* m_camera;
			GeometryBufferContainer* geomBuff;

			bool running = true;

			ID3D11Device* device;
			ID3D11DeviceContext* devContext;
			IDXGISwapChain* swapChain;
			ID3D11RenderTargetView* backBuffer;
			ID3D11RenderTargetView* bloomBuffer;
			ID3D11RenderTargetView* bloomBuffer2;
			ID3D11DepthStencilView* depthBuffer;
			ID3D11DepthStencilState* depthState;
			ID3D11DepthStencilState* depthStateDisabled;
			ID3D11RasterizerState* rasterizerState;

			ID3D11VertexShader* bloomVtx;
			ID3D11PixelShader* bloomPx;
			ID3D11PixelShader* bloomPxV;
			ID3D11ShaderResourceView* bloomSRV;
			ID3D11ShaderResourceView* bloomSRV2;

			ID3D11BlendState* blendAdd;
			ID3D11BlendState* blendState;

			ID3D11VertexShader* dpthVtx;
			ID3D11PixelShader* dpthPx;
			ID3D11InputLayout* dpthILayout;

			ID3D11Buffer* lightBuff;
			ID3D11ShaderResourceView* lightUAV;

			GraphicsController(Engine*);
			~GraphicsController();

			GraphicsController(const GraphicsController&) = delete;
			GraphicsController& operator=(const GraphicsController&) = delete;

			void StartDraw();
			void RenderObjects();
			void EndDraw();

			void AddRenderer(Renderer*);
			void RemoveRenderer(Renderer*);

			void FillBuffers(Renderer*, bool);

			bool HandleMessage(HWND, UINT, WPARAM, LPARAM);

			void SetCamera(Camera*);
			void AddLight(Light*);
			void RemoveLight(Light*);

			void RenderLightDepth(Light*);
			void DisableDepthWrite();
			void EnableDepthWrite();

			void ShadowPasses();
			void ConstructLightBuffer();

			//void Draw();
		};
	}
}

#endif
