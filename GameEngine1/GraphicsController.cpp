#include "GraphicsController.h"
#include <comdef.h>
#include <iostream>
#include <locale>
#include <Windows.h>
#include "CompositeObject.h"
#include "Transform.h"

using namespace GameEngine;
using namespace Graphics;

typedef struct
{
	DirectX::XMFLOAT4X4A wv;
	DirectX::XMFLOAT4X4A vp;
	DirectX::XMFLOAT3A camPos;
	float padding;
} PerFrameBuffer;

typedef struct
{
	DirectX::XMFLOAT4X4A v;
	DirectX::XMFLOAT4X4A p;
	DirectX::XMFLOAT3A lightPos;
	float padding;
} LightBufferS;

GraphicsController::GraphicsController(int w, int h, bool fullscreen, HWND wnd) : hWnd(wnd), m_fullscreen(fullscreen),
                                                                                  m_scrWidth(w), m_scrHeight(h)
{
	//instance = this;
	
	//Create device and device context
	D3D_FEATURE_LEVEL featureLevel;
	D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		nullptr,
		NULL,
		D3D11_SDK_VERSION,
		&device,
		&featureLevel,
		&devContext
	);

	if (featureLevel < D3D_FEATURE_LEVEL_11_0)
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
	swpDesc.SampleDesc.Count = 1;
	swpDesc.SampleDesc.Quality = 0;
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
	device->CreateRenderTargetView(pBackBuffer, nullptr, &backBuffer);
	pBackBuffer->Release();

	D3D11_TEXTURE2D_DESC tD;
	ZeroMemory(&tD, sizeof(D3D11_TEXTURE2D_DESC));
	tD.ArraySize = 1;
	tD.Usage = D3D11_USAGE_DEFAULT;
	tD.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	tD.Height = m_scrHeight;
	tD.Width = m_scrWidth;
	tD.MiscFlags = 0;
	tD.MipLevels = 1;
	tD.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tD.SampleDesc.Count = 1;
	tD.SampleDesc.Quality = 0;

	device->CreateTexture2D(&tD, nullptr, &pBackBuffer);
	device->CreateRenderTargetView(pBackBuffer, nullptr, &bloomBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvD;
	srvD.Format = tD.Format;
	srvD.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvD.Texture2D.MipLevels = -1;
	srvD.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(pBackBuffer, nullptr, &bloomSRV);

	device->CreateTexture2D(&tD, nullptr, &pBackBuffer);
	device->CreateRenderTargetView(pBackBuffer, nullptr, &bloomBuffer2);
	device->CreateShaderResourceView(pBackBuffer, nullptr, &bloomSRV2);

	//Create depth and stencil buffer
	D3D11_TEXTURE2D_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.ArraySize = 1;
	depthDesc.MipLevels = 1;
	depthDesc.Height = m_scrHeight;
	depthDesc.Width = m_scrWidth;

	ID3D11Texture2D* pDepthBuffer;
	device->CreateTexture2D(&depthDesc, nullptr, &pDepthBuffer);

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

	dsD.DepthEnable = true;
	dsD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsD.DepthFunc = D3D11_COMPARISON_LESS;

	dsD.StencilEnable = false;

	device->CreateDepthStencilState(&dsD, &depthStateDisabled);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvD;
	ZeroMemory(&dsvD, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	dsvD.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvD.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvD.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(pDepthBuffer, &dsvD, &depthBuffer);
	pDepthBuffer->Release();

	//Set back buffer & depth/stencil view as render targets
	ID3D11RenderTargetView* rTVs[2] = {backBuffer, bloomBuffer};
	devContext->OMSetRenderTargets(2, rTVs, depthBuffer);

	//Create rasterizer state
	D3D11_RASTERIZER_DESC rsD;
	ZeroMemory(&rsD, sizeof(D3D11_RASTERIZER_DESC));

	rsD.AntialiasedLineEnable = false;
	rsD.CullMode = D3D11_CULL_BACK;
	rsD.DepthBias = 0;
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
	ZeroMemory(&vP, sizeof(D3D11_VIEWPORT));

	vP.TopLeftX = 0;
	vP.TopLeftY = 0;
	vP.MinDepth = 0.0;
	vP.MaxDepth = 1.0;
	vP.Height = static_cast<float>(h);
	vP.Width = static_cast<float>(w);

	devContext->RSSetViewports(1, &vP);

	//Create index/vertex/constant buffers
	/*D3D11_BUFFER_DESC vBD;
	ZeroMemory(&vBD, sizeof(D3D11_BUFFER_DESC));
	vBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBD.ByteWidth = sizeof(Vertex) * 200000;
	vBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vBD.Usage = D3D11_USAGE_DYNAMIC;

	device->CreateBuffer(&vBD, NULL, &vertexBuffer);

	D3D11_BUFFER_DESC iBD;
	ZeroMemory(&iBD, sizeof(D3D11_BUFFER_DESC));
	iBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	iBD.ByteWidth = sizeof(unsigned) * 210000;
	iBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	iBD.Usage = D3D11_USAGE_DYNAMIC;

	device->CreateBuffer(&iBD, NULL, &indexBuffer);*/

	//Per frame CB
	D3D11_BUFFER_DESC cBD;
	ZeroMemory(&cBD, sizeof(D3D11_BUFFER_DESC));
	cBD.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cBD.ByteWidth = sizeof(PerFrameBuffer);
	cBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cBD.Usage = D3D11_USAGE_DYNAMIC;

	device->CreateBuffer(&cBD, nullptr, &cBufferFrame);
	device->CreateBuffer(&cBD, nullptr, &cBufferLight);

	//Per object CB
	cBD.ByteWidth = sizeof(DirectX::XMFLOAT4X4A);

	device->CreateBuffer(&cBD, nullptr, &cBufferObject);

	geomBuff = new GeometryBufferContainer(device, devContext);

	D3D11_BLEND_DESC bDsc;
	ZeroMemory(&bDsc, sizeof(D3D11_BLEND_DESC));
	bDsc.AlphaToCoverageEnable = false;
	bDsc.IndependentBlendEnable = false;
	bDsc.RenderTarget[0].BlendEnable = true;
	bDsc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bDsc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	bDsc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bDsc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	bDsc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bDsc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	bDsc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bDsc.RenderTarget[1].BlendEnable = true;
	bDsc.RenderTarget[1].BlendOp = D3D11_BLEND_OP_ADD;
	bDsc.RenderTarget[1].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	bDsc.RenderTarget[1].DestBlend = D3D11_BLEND_ZERO;
	bDsc.RenderTarget[1].DestBlendAlpha = D3D11_BLEND_ZERO;
	bDsc.RenderTarget[1].SrcBlend = D3D11_BLEND_ONE;
	bDsc.RenderTarget[1].SrcBlendAlpha = D3D11_BLEND_ONE;
	bDsc.RenderTarget[1].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	device->CreateBlendState(&bDsc, &blendState);
	devContext->OMSetBlendState(blendState, nullptr, 0xffffffff);

	bDsc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	bDsc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	device->CreateBlendState(&bDsc, &blendAdd);
}

GraphicsController::~GraphicsController()
{
	vertexBuffer->Release();
	indexBuffer->Release();
	cBufferFrame->Release();
	cBufferObject->Release();
	
	delete geomBuff;

	device->Release();
	devContext->Release();
	swapChain->Release();
	backBuffer->Release();
	depthBuffer->Release();
}

void GraphicsController::StartDraw()
{
	ConstructLightBuffer();
	devContext->PSSetShaderResources(2, 1, &lightUAV);
	devContext->ClearRenderTargetView(backBuffer, DirectX::XMVECTORF32{0.0, 0.0, 0.0, 1.0});
	devContext->ClearRenderTargetView(bloomBuffer, DirectX::XMVECTORF32{0.0, 0.0, 0.0, 1.0});
	devContext->ClearRenderTargetView(bloomBuffer2, DirectX::XMVECTORF32{0.0, 0.0, 0.0, 1.0});
	devContext->ClearDepthStencilView(depthBuffer,
	                                  D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
	                                  1.0, 0);
	devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devContext->RSSetViewports(1, &vP);
	devContext->OMSetDepthStencilState(depthState, 1);

	//Fill per frame buffer
	PerFrameBuffer data;

	//Get WV matrix
	DirectX::XMMATRIX mat = m_camera->obj->GetComponent<Elements::Transform>()->GetTransform();
	DirectX::XMVECTOR det = XMMatrixDeterminant(mat);

	XMStoreFloat4x4A(&data.wv, XMMatrixTranspose(XMMatrixInverse(&det, mat)));
	XMStoreFloat4x4A(&data.vp, XMMatrixTranspose(
		                 DirectX::XMMatrixPerspectiveFovLH(0.82547f, static_cast<float>(m_scrWidth) / m_scrHeight, 8.0f,
		                                                   2000.0f)));
	XMStoreFloat3A(&data.camPos, m_camera->obj->GetComponent<Elements::Transform>()->GetPosition());

	D3D11_MAPPED_SUBRESOURCE mp;
	devContext->Map(cBufferFrame, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mp);
	memcpy(mp.pData, &data, sizeof(PerFrameBuffer));
	devContext->Unmap(cBufferFrame, 0);

	devContext->VSSetConstantBuffers(0, 1, &cBufferFrame);

	DirectX::XMVECTOR viewDir = m_camera->obj->GetComponent<Elements::Transform>()->GetForwards();
	rq.Refresh(viewDir);

	//Fill light buffer
	LightBufferS light;

	//Get WV matrix
	mat = m_light->obj->GetComponent<Elements::Transform>()->GetTransform();
	det = XMMatrixDeterminant(mat);

	XMStoreFloat4x4A(&light.v, XMMatrixTranspose(XMMatrixInverse(&det, mat)));
	XMStoreFloat4x4A(
		&light.p, XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1.0f, 8.0f, 800.0f)));
	XMStoreFloat3A(&light.lightPos, m_light->obj->GetComponent<Elements::Transform>()->GetPosition());

	devContext->Map(cBufferLight, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mp);
	memcpy(mp.pData, &light, sizeof(LightBufferS));
	devContext->Unmap(cBufferLight, 0);

	devContext->VSSetConstantBuffers(2, 1, &cBufferLight);

	ID3D11ShaderResourceView* srv = m_light->GetSRV();
	devContext->PSSetShaderResources(1, 1, &srv);
	devContext->PSSetSamplers(1, 1, &m_light->ss);

	devContext->VSSetConstantBuffers(1, 1, &cBufferObject);
}

void GraphicsController::RenderObjects()
{
	for (auto r : rq)
	{
		if (!r->m_active) continue;
		FillBuffers(r, true);
	}
}

void GraphicsController::EndDraw()
{
	devContext->ClearDepthStencilView(depthBuffer,
	                                  D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
	                                  1.0, 0);
	ID3D11RenderTargetView* rTVs[2] = {bloomBuffer2, nullptr};
	devContext->OMSetRenderTargets(2, rTVs, depthBuffer);
	devContext->PSSetShaderResources(0, 1, &bloomSRV);
	devContext->VSSetShader(bloomVtx, nullptr, NULL);
	devContext->PSSetShader(bloomPx, nullptr, NULL);
	devContext->OMSetBlendState(blendAdd, nullptr, 0xffffffff);
	devContext->Draw(3, 0);

	devContext->ClearDepthStencilView(depthBuffer,
	                                  D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
	                                  1.0, 0);

	rTVs[0] = backBuffer;
	devContext->OMSetRenderTargets(2, rTVs, depthBuffer);
	devContext->PSSetShaderResources(0, 1, &bloomSRV2);
	devContext->PSSetShader(bloomPxV, nullptr, NULL);
	devContext->Draw(3, 0);

	devContext->OMSetBlendState(blendState, nullptr, 0xffffffff);
	swapChain->Present(NULL, NULL);
	ID3D11ShaderResourceView* nullSRV = nullptr;
	devContext->PSSetShaderResources(1, 1, &nullSRV);
	devContext->PSSetShaderResources(0, 1, &nullSRV);
}

void GraphicsController::AddRenderer(Renderer* r)
{
	//renderers.Insert(std::make_pair(renderers.Size(), r));
	rq.AddRenderer(r);
	geomBuff->AddRenderer(r);
}

void GraphicsController::RemoveRenderer(Renderer* r)
{
	rq.RemoveRenderer(r);
}

void GraphicsController::FillBuffers(Renderer* r, bool tex)
{
	D3D11_MAPPED_SUBRESOURCE mp;

	if (r->GetTransparent()) DisableDepthWrite();

	/*devContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mp);
	memcpy(mp.pData, &(*r->mesh)[i].vertices[0], (*r->mesh)[i].vertices.size() * sizeof(Vertex));
	devContext->Unmap(vertexBuffer, 0);

	devContext->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mp);
	memcpy(mp.pData, &(*r->mesh)[i].indices[0], (*r->mesh)[i].indices.size() * sizeof(unsigned));
	devContext->Unmap(indexBuffer, 0);*/

	DirectX::XMFLOAT4X4A data[1];
	XMStoreFloat4x4A(data, XMMatrixTranspose(r->obj->GetComponent<Elements::Transform>()->GetTransform()));

	devContext->Map(cBufferObject, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mp);
	memcpy(mp.pData, data, sizeof(DirectX::XMFLOAT4X4A));
	devContext->Unmap(cBufferObject, 0);

	if (tex)
	{
		ID3D11SamplerState* smpl = r->mat->GetSampler();
		devContext->PSSetSamplers(0, 1, &smpl);
		r->Render(0);
	}
	else
	{
		GeometryBuffer::BufferLocation idxes = geomBuff->FindRenderer(r);
		devContext->DrawIndexed(r->mesh->indices.size(), std::get<0>(idxes), std::get<1>(idxes));
	}
}

//Handle window resizing
bool GraphicsController::HandleMessage(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message != WM_SIZE) return false;
	m_scrWidth = LOWORD(lParam);
	m_scrHeight = HIWORD(lParam);

	if (m_scrWidth == 0 || m_scrHeight == 0) return true;

	//Release old resources
	devContext->OMSetRenderTargets(0, nullptr, nullptr);
	depthBuffer->Release();
	backBuffer->Release();

	swapChain->ResizeBuffers(0, m_scrWidth, m_scrHeight, DXGI_FORMAT_UNKNOWN, 0);
	ID3D11Texture2D* pBackBuffer;
	swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	device->CreateRenderTargetView(pBackBuffer, nullptr, &backBuffer);
	pBackBuffer->Release();

	//Create new depth/stencil buffer
	D3D11_TEXTURE2D_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.ArraySize = 1;
	depthDesc.MipLevels = 1;
	depthDesc.Height = m_scrHeight;
	depthDesc.Width = m_scrWidth;

	ID3D11Texture2D* pDepthBuffer;
	device->CreateTexture2D(&depthDesc, nullptr, &pDepthBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvD;
	ZeroMemory(&dsvD, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	dsvD.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvD.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvD.Texture2D.MipSlice = 0;

	device->CreateDepthStencilView(pDepthBuffer, &dsvD, &depthBuffer);
	pDepthBuffer->Release();

	D3D11_TEXTURE2D_DESC tD;
	ZeroMemory(&tD, sizeof(D3D11_TEXTURE2D_DESC));
	tD.ArraySize = 1;
	tD.Usage = D3D11_USAGE_DEFAULT;
	tD.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	tD.Height = m_scrHeight;
	tD.Width = m_scrWidth;
	tD.MiscFlags = 0;
	tD.MipLevels = 1;
	tD.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tD.SampleDesc.Count = 1;
	tD.SampleDesc.Quality = 0;

	device->CreateTexture2D(&tD, nullptr, &pBackBuffer);
	device->CreateRenderTargetView(pBackBuffer, nullptr, &bloomBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvD;
	srvD.Format = tD.Format;
	srvD.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvD.Texture2D.MipLevels = -1;
	srvD.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(pBackBuffer, nullptr, &bloomSRV);

	device->CreateTexture2D(&tD, nullptr, &pBackBuffer);
	device->CreateRenderTargetView(pBackBuffer, nullptr, &bloomBuffer2);
	device->CreateShaderResourceView(pBackBuffer, nullptr, &bloomSRV2);

	ID3D11RenderTargetView* rTVs[2] = {backBuffer, bloomBuffer};
	devContext->OMSetRenderTargets(2, rTVs, depthBuffer);

	vP.TopLeftX = 0;
	vP.TopLeftY = 0;
	vP.MinDepth = 0.0;
	vP.MaxDepth = 1.0;
	vP.Height = static_cast<float>(m_scrHeight);
	vP.Width = static_cast<float>(m_scrWidth);
	devContext->RSSetViewports(1, &vP);

	return true;
}

void GraphicsController::SetCamera(Camera* cam)
{
	m_camera = cam;
}

void GraphicsController::AddLight(Light* light)
{
	m_lights.push_back(light);
	if (light->m_shadows) m_light = light;
}

void GraphicsController::RemoveLight(Light* light)
{
	auto it = std::find(m_lights.begin(), m_lights.end(), light);
	m_lights.erase(it);
}

void GraphicsController::ShadowPasses()
{
	for (Light* l : m_lights)
	{
		if (!l->m_shadows) continue;

		RenderLightDepth(l);
	}
}

void GraphicsController::ConstructLightBuffer()
{
	if (lightBuff != nullptr) lightBuff->Release();
	if (lightUAV != nullptr) lightUAV->Release();

	LightBuffer* data = new LightBuffer[m_lights.size()];
	for (int i = 0; i < m_lights.size(); ++i)
		m_lights[i]->FillLightBuffer(data + i);
	
	D3D11_BUFFER_DESC bD;
	ZeroMemory(&bD, sizeof(bD));
	bD.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bD.Usage = D3D11_USAGE_DYNAMIC;
	bD.StructureByteStride = sizeof(LightBuffer);
	bD.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bD.ByteWidth = m_lights.size() * sizeof(LightBuffer);

	D3D11_SUBRESOURCE_DATA bufferInitData;
	ZeroMemory((&bufferInitData), sizeof(bufferInitData));
	bufferInitData.pSysMem = data;

	device->CreateBuffer(&bD, &bufferInitData, &lightBuff);

	D3D11_SHADER_RESOURCE_VIEW_DESC uavD;
	ZeroMemory(&uavD, sizeof(uavD));
	uavD.Format = DXGI_FORMAT_UNKNOWN;
	uavD.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	uavD.Buffer.ElementWidth = m_lights.size();
	device->CreateShaderResourceView(lightBuff, &uavD, &lightUAV);
/*
	D3D11_MAPPED_SUBRESOURCE mSR;
	devContext->Map(lightBuff, 0, D3D11_MAP_WRITE_DISCARD, 0, &mSR);
	memcpy(mSR.pData, data, sizeof(LightBuffer) * m_lights.size());
	devContext->Unmap(lightBuff, 0);*/
}

void GraphicsController::RenderLightDepth(Light* light)
{
	ID3D11RenderTargetView* rtv = m_light->GetRTV();
	devContext->OMSetRenderTargets(1, &rtv, m_light->depthBuff);
	devContext->IASetInputLayout(dpthILayout);
	devContext->VSSetShader(dpthVtx, nullptr, NULL);
	devContext->PSSetShader(dpthPx, nullptr, NULL);
	devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	devContext->RSSetViewports(1, &m_light->vp);
	devContext->ClearDepthStencilView(m_light->depthBuff,
	                                  D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
	                                  1.0, 0);
	devContext->ClearRenderTargetView(m_light->GetRTV(), DirectX::XMVECTORF32{1.0, 1.0, 1.0, 1.0});

	//Fill per frame buffer
	PerFrameBuffer data;

	//Get WV matrix
	DirectX::XMMATRIX mat = m_light->obj->GetComponent<Elements::Transform>()->GetTransform();
	DirectX::XMVECTOR det = XMMatrixDeterminant(mat);

	XMStoreFloat4x4A(&data.wv, XMMatrixTranspose(XMMatrixInverse(&det, mat)));
	XMStoreFloat4x4A(
		&data.vp, XMMatrixTranspose(DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1.0f, 8.0f, 800.0f)));

	D3D11_MAPPED_SUBRESOURCE mp;
	devContext->Map(cBufferFrame, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mp);
	memcpy(mp.pData, &data, sizeof(DirectX::XMFLOAT4X4A) * 2);
	devContext->Unmap(cBufferFrame, 0);

	devContext->VSSetConstantBuffers(0, 1, &cBufferFrame);

	for (auto r : rq)
	{
		if (!r->m_active) continue;
		if (r->GetTransparent()) continue;
		FillBuffers(r, false);
	}

	ID3D11RenderTargetView* rTVs[2] = {backBuffer, bloomBuffer};
	devContext->OMSetRenderTargets(2, rTVs, depthBuffer);
}

void GraphicsController::EnableDepthWrite()
{
	devContext->OMSetDepthStencilState(depthState, 1);
}

void GraphicsController::DisableDepthWrite()
{
	//devContext->OMSetDepthStencilState(depthStateDisabled, 1);
}
