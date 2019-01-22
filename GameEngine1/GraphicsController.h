#ifndef __GRAPHICS_CONTROLLER_INCLUDED__
#define __GRAPHICS_CONTROLLER_INCLUDED__

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <windows.h>
#include <windowsx.h>
#include <d3dcompiler.h>
#include <string>
#include <vector>
#include "Renderer.h"
#include "Material.h"
#include "Camera.h"
#include "Light.h"
#include "GeometryBufferContainer.h"
#include "ComponentContainers.h"
#include "PriorityQueue.h"
#include "RenderQueue.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

namespace GameEngine
{
	namespace Graphics
	{

		enum SHADER_TYPE { Shader_Type_Vertex, Shader_Type_Pixel };

		class GraphicsController
		{
			HWND hWnd;
			HINSTANCE hInstance;
			bool m_fullscreen;

			ID3D11Buffer* vertexBuffer;
			ID3D11Buffer* indexBuffer;
			ID3D11Buffer* cBufferFrame;
			ID3D11Buffer* cBufferObject;
			ID3D11Buffer* cBufferLight;



			//Utils::PriorityQueue<GameEngine::Renderer*> renderers;
		public:
			RenderQueue rq;
		private:
			UINT numQualityLevels;
		public:
			Camera* m_camera;
		private:
			Light* m_light;
			D3D11_VIEWPORT vP;

		public:
			GeometryBufferContainer* geomBuff;

			int m_scrWidth;
			int m_scrHeight;
			//static GraphicsController* instance;

			bool running = true;

			ID3D11Device* device;
			ID3D11DeviceContext* devContext;
			IDXGISwapChain* swapChain;
			ID3D11RenderTargetView* backBuffer;
			ID3D11DepthStencilView* depthBuffer;
			ID3D11DepthStencilState* depthState;
			ID3D11DepthStencilState* depthStateDisabled;
			ID3D11RasterizerState* rasterizerState;

			ID3D11VertexShader* dpthVtx;
			ID3D11PixelShader* dpthPx;
			ID3D11InputLayout* dpthILayout;

			GraphicsController(int, int, bool, HWND);
			~GraphicsController();

			GraphicsController(const GraphicsController&) = delete;
			GraphicsController& operator=(const GraphicsController&) = delete;

			void StartDraw();
			void RenderObjects();
			void EndDraw();

			void AddRenderer(GameEngine::Renderer*);
			void RemoveRenderer(GameEngine::Renderer*);

			void FillBuffers(GameEngine::Renderer*, bool);

			bool HandleMessage(HWND, UINT, WPARAM, LPARAM);

			void SetCamera(Camera*);
			void SetLight(Light*);

			void RenderLightDepth();
			void DisableDepthWrite();
			void EnableDepthWrite();
		};
	}
}

#endif