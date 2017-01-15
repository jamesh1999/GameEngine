#include "GraphicsController.h"
#include <iostream>
#include <comdef.h>
#include <locale>
#include <codecvt>
#include <Windows.h>
#include <windowsx.h>
#include "Transform.h"
#include "CompositeObject.h"

GraphicsController* GraphicsController::instance = NULL;

typedef struct
{
	DirectX::XMFLOAT4X4A wv;
	DirectX::XMFLOAT4X4A vp;
	DirectX::XMFLOAT3A camPos;
	float padding;
} PerFrameBuffer;

GraphicsController::GraphicsController(int w, int h, bool fullscreen, HWND wnd)
	: hWnd(wnd), m_scrWidth(w), m_scrHeight(h), m_fullscreen(fullscreen)
{
	instance = this;

	//Create device and device context
	D3D_FEATURE_LEVEL featureLevel;
	D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		D3D11_CREATE_DEVICE_DEBUG,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&device,
		&featureLevel,
		&devContext
	);

	if(featureLevel < D3D_FEATURE_LEVEL_11_0)
	{
		std::cerr << "The graphics hardware does not support DirectX 11" << std::endl;
		exit(-1);
	}

	//Create Swap chain
	device->CheckMultisampleQualityLevels(DXGI_FORMAT_B8G8R8A8_UNORM, 4, &numQualityLevels);

	DXGI_SWAP_CHAIN_DESC swpDesc;
	ZeroMemory(&swpDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swpDesc.BufferCount = 1; //Double buffered
	swpDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	swpDesc.BufferDesc.Height = m_scrHeight;
	swpDesc.BufferDesc.Width = m_scrWidth;
	swpDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swpDesc.OutputWindow = hWnd;
	swpDesc.SampleDesc.Count = 4;
	swpDesc.SampleDesc.Quality = numQualityLevels - 1;
	swpDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swpDesc.Windowed = !m_fullscreen;
	swpDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	IDXGIDevice* dxgiDevice;
	device->QueryInterface(__uuidof(IDXGIDevice), (LPVOID*)&dxgiDevice);

	IDXGIAdapter* dxgiAdapter;
	dxgiDevice->GetAdapter(&dxgiAdapter);

	IDXGIFactory* dxgiFactory;
	dxgiAdapter->GetParent(__uuidof(IDXGIFactory), (LPVOID*)&dxgiFactory);

	dxgiFactory->CreateSwapChain(device, &swpDesc, &swapChain);

	//Release COM objects
	dxgiDevice->Release();
	dxgiAdapter->Release();
	dxgiFactory->Release();

	//Bind backbuffer
	ID3D11Texture2D* pBackBuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	device->CreateRenderTargetView(pBackBuffer, NULL, &backBuffer);
	pBackBuffer->Release();

	//Create depth and stencil buffer
	D3D11_TEXTURE2D_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.SampleDesc.Count = 4;
	depthDesc.SampleDesc.Quality = numQualityLevels - 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.ArraySize = 1;
	depthDesc.MipLevels = 1;
	depthDesc.Height = m_scrHeight;
	depthDesc.Width = m_scrWidth;

	ID3D11Texture2D* pDepthBuffer;
	device->CreateTexture2D(&depthDesc, NULL, &pDepthBuffer);

	D3D11_DEPTH_STENCIL_DESC dsD;
	ZeroMemory(&dsD, sizeof(D3D11_DEPTH_STENCIL_DESC));

	dsD.DepthEnable = true;
	dsD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsD.DepthFunc = D3D11_COMPARISON_LESS;

	dsD.StencilEnable = true;
	dsD.StencilReadMask = 0xFF;
	dsD.StencilWriteMask = 0xFF;
	dsD.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsD.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsD.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsD.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	dsD.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsD.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsD.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsD.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	device->CreateDepthStencilState(&dsD, &depthState);
	devContext->OMSetDepthStencilState(depthState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvD;
	ZeroMemory(&dsvD, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	dsvD.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvD.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvD.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(pDepthBuffer, &dsvD, &depthBuffer);
	pDepthBuffer->Release();

	//Set back buffer & depth/stencil view as render targets
	devContext->OMSetRenderTargets(1, &backBuffer, depthBuffer);

	//Create rasterizer state
	D3D11_RASTERIZER_DESC rsD;
	ZeroMemory(&rsD, sizeof(D3D11_RASTERIZER_DESC));

	rsD.AntialiasedLineEnable = false;
	rsD.CullMode = D3D11_CULL_BACK;
	rsD.DepthBias = 0.0;
	rsD.DepthBiasClamp = 0.0;
	rsD.DepthClipEnable = true;
	rsD.FillMode = D3D11_FILL_SOLID;
	rsD.FrontCounterClockwise = false;
	rsD.MultisampleEnable = true;
	rsD.ScissorEnable = false;
	rsD.SlopeScaledDepthBias = false;

	device->CreateRasterizerState(&rsD, &rasterizerState);
	devContext->RSSetState(rasterizerState);

	//Setup viewport
	D3D11_VIEWPORT vP;

	ZeroMemory(&vP, sizeof(D3D11_VIEWPORT));

	vP.TopLeftX = 0;
	vP.TopLeftY = 0;
	vP.MinDepth = 0.0;
	vP.MaxDepth = 1.0;
	vP.Height = h;
	vP.Width = w;

	devContext->RSSetViewports(1, &vP);

	//Create index/vertex/constant buffers
	D3D11_BUFFER_DESC vBD;
	ZeroMemory(&vBD, sizeof(D3D11_BUFFER_DESC));
	vBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBD.ByteWidth = sizeof(Vertex) * 150000;
	vBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vBD.Usage = D3D11_USAGE_DYNAMIC;

	device->CreateBuffer(&vBD, NULL, &vertexBuffer);

	D3D11_BUFFER_DESC iBD;
	ZeroMemory(&iBD, sizeof(D3D11_BUFFER_DESC));
	iBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBD.ByteWidth = sizeof(unsigned) * 210000;
	iBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	iBD.Usage = D3D11_USAGE_DYNAMIC;

	device->CreateBuffer(&iBD, NULL, &indexBuffer);


	//Per frame CB
	D3D11_BUFFER_DESC cBD;
	ZeroMemory(&cBD, sizeof(D3D11_BUFFER_DESC));
	cBD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBD.ByteWidth = sizeof(PerFrameBuffer);
	cBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBD.Usage = D3D11_USAGE_DYNAMIC;

	device->CreateBuffer(&cBD, NULL, &cBufferFrame);

	//Per object CB
	cBD.ByteWidth = sizeof(DirectX::XMFLOAT4X4A);

	device->CreateBuffer(&cBD, NULL, &cBufferObject);
}

GraphicsController::~GraphicsController()
{
	vertexBuffer->Release();
	indexBuffer->Release();
	cBufferFrame->Release();
	cBufferObject->Release();

	device->Release();
	devContext->Release();
	swapChain->Release();
	backBuffer->Release();
	depthBuffer->Release();

	for (auto r : renderers)
		delete r;

	renderers.clear();
}

void GraphicsController::StartDraw()
{
	devContext->ClearRenderTargetView(backBuffer, DirectX::XMVECTORF32{ 0.0, 0.0, 0.0, 1.0 });
	devContext->ClearDepthStencilView(depthBuffer,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0, 0);
	devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Fill per frame buffer
	PerFrameBuffer data;

	//Get WV matrix
	DirectX::XMMATRIX mat = m_camera->obj->GetComponent<Transform>()->GetTransform();
	DirectX::XMVECTOR det = DirectX::XMMatrixDeterminant(mat);

	DirectX::XMStoreFloat4x4A(&data.wv, DirectX::XMMatrixTranspose(DirectX::XMMatrixInverse(&det, mat)));
	DirectX::XMStoreFloat4x4A(&data.vp, DirectX::XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(0.82547f, static_cast<float>(m_scrWidth) / m_scrHeight, 8.0f, 2000.0f)));
	DirectX::XMStoreFloat3A(&data.camPos, m_camera->obj->GetComponent<Transform>()->GetPosition());

	D3D11_MAPPED_SUBRESOURCE mp;
	devContext->Map(cBufferFrame, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mp);
	memcpy(mp.pData, &data, sizeof(DirectX::XMFLOAT4X4A) * 2);
	devContext->Unmap(cBufferFrame, 0);

	devContext->VSSetConstantBuffers(0, 1, &cBufferFrame);
}

void GraphicsController::RenderObjects()
{
	for(auto r : renderers)
	{
		FillBuffers(r);
	}
}

void GraphicsController::EndDraw()
{
	swapChain->Present(NULL, NULL);
}

void GraphicsController::AddRenderer(Renderer* r)
{
	renderers.push_back(r);
}

void GraphicsController::FillBuffers(Renderer* r)
{
	D3D11_MAPPED_SUBRESOURCE mp;

	for (int i = 0; i < r->mesh->size(); ++i)
	{
		devContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mp);
		memcpy(mp.pData, &(*r->mesh)[i].vertices[0], (*r->mesh)[i].vertices.size() * sizeof(Vertex));
		devContext->Unmap(vertexBuffer, 0);

		devContext->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mp);
		memcpy(mp.pData, &(*r->mesh)[i].indices[0], (*r->mesh)[i].indices.size() * sizeof(unsigned));
		devContext->Unmap(indexBuffer, 0);

		DirectX::XMFLOAT4X4A data[1];
		DirectX::XMStoreFloat4x4A(data, DirectX::XMMatrixTranspose(r->obj->GetComponent<Transform>()->GetTransform()));

		devContext->Map(cBufferObject, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mp);
		memcpy(mp.pData, data, sizeof(DirectX::XMFLOAT4X4A));
		devContext->Unmap(cBufferObject, 0);

		unsigned stride = sizeof(Vertex);
		unsigned offset = 0;
		devContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
		devContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
		devContext->VSSetConstantBuffers(1, 1, &cBufferObject);

		devContext->PSSetSamplers(0, 1, &r->mat[i]->m_samplerState);
		devContext->PSSetShaderResources(0, 1, &r->mat[i]->m_texView);
		r->Render(i);
	}
}

//Handle window resizing
bool GraphicsController::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message != WM_SIZE || this == nullptr) return false;
	m_scrWidth = LOWORD(lParam);
	m_scrHeight = HIWORD(lParam);

	if (m_scrWidth == 0 || m_scrHeight == 0) return true;

	//Release old resources
	devContext->OMSetRenderTargets(0, NULL, NULL);
	depthBuffer->Release();
	backBuffer->Release();

	swapChain->ResizeBuffers(0, m_scrWidth, m_scrHeight, DXGI_FORMAT_UNKNOWN, 0);
	ID3D11Texture2D* pBackBuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	device->CreateRenderTargetView(pBackBuffer, NULL, &backBuffer);
	pBackBuffer->Release();

	//Create new depth/stencil buffer
	D3D11_TEXTURE2D_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.SampleDesc.Count = 4;
	depthDesc.SampleDesc.Quality = numQualityLevels - 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.ArraySize = 1;
	depthDesc.MipLevels = 1;
	depthDesc.Height = m_scrHeight;
	depthDesc.Width = m_scrWidth;

	ID3D11Texture2D* pDepthBuffer;
	device->CreateTexture2D(&depthDesc, NULL, &pDepthBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvD;
	ZeroMemory(&dsvD, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	dsvD.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvD.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
	dsvD.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(pDepthBuffer, &dsvD, &depthBuffer);
	pDepthBuffer->Release();

	devContext->OMSetRenderTargets(1, &backBuffer, depthBuffer);

	D3D11_VIEWPORT vP;
	vP.TopLeftX = 0;
	vP.TopLeftY = 0;
	vP.MinDepth = 0.0;
	vP.MaxDepth = 1.0;
	vP.Height = m_scrHeight;
	vP.Width = m_scrWidth;
	devContext->RSSetViewports(1, &vP);

	return true;
}

void GraphicsController::SetCamera(Camera *cam)
{
	m_camera = cam;
}
