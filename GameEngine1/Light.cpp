#include "Light.h"
#include "GraphicsController.h"
#include <minwinbase.h>
#include <minwinbase.h>

void Light::Create()
{
	D3D11_TEXTURE2D_DESC tDsc;
	ZeroMemory(&tDsc, sizeof(tDsc));
	tDsc.ArraySize = 1;
	tDsc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	tDsc.CPUAccessFlags = 0;
	tDsc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tDsc.Height = 4096;
	tDsc.Width = 4096;
	tDsc.MipLevels = 1;
	tDsc.MiscFlags = 0;
	tDsc.SampleDesc.Count = 1;
	tDsc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_RENDER_TARGET_VIEW_DESC rtvDsc;
	ZeroMemory(&rtvDsc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rtvDsc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	rtvDsc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDsc.Texture2D.MipSlice = 0;

	D3D11_SHADER_RESOURCE_VIEW_DESC srDsc;
	ZeroMemory(&srDsc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srDsc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	srDsc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srDsc.Texture2D.MipLevels = 1;
	srDsc.Texture2D.MostDetailedMip = 0;

	engine->graphics->device->CreateTexture2D(&tDsc, NULL, &depthTex);
	engine->graphics->device->CreateRenderTargetView(depthTex, &rtvDsc, &renderTarget);
	engine->graphics->device->CreateShaderResourceView(depthTex, &srDsc, &shaderResource);



	//Create depth and stencil buffer
	D3D11_TEXTURE2D_DESC depthDesc;
	ZeroMemory(&depthDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthDesc.ArraySize = 1;
	depthDesc.MipLevels = 1;
	depthDesc.Height = 4096;
	depthDesc.Width = 4096;

	engine->graphics->device->CreateTexture2D(&depthDesc, NULL, &depthBuffer);

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvD;
	ZeroMemory(&dsvD, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	dsvD.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvD.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvD.Texture2D.MipSlice = 0;

	engine->graphics->device->CreateDepthStencilView(depthBuffer, &dsvD, &depthBuff);

	ZeroMemory(&vp, sizeof(D3D11_VIEWPORT));
	vp.Width = 4096;
	vp.Height = 4096;
	vp.MaxDepth = 1.0;
	vp.MinDepth = 0.0;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;


	D3D11_SAMPLER_DESC sD;
	sD.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
	sD.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	sD.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	sD.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	sD.MipLODBias = 0.0f;
	sD.MaxAnisotropy = 1;
	sD.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sD.BorderColor[0] = 0;
	sD.BorderColor[1] = 0;
	sD.BorderColor[2] = 0;
	sD.BorderColor[3] = 0;
	sD.MinLOD = 0;
	sD.MaxLOD = D3D11_FLOAT32_MAX;

	engine->graphics->device->CreateSamplerState(&sD, &ss);
}

Light::~Light()
{
	depthTex->Release();
	renderTarget->Release();
	shaderResource->Release();
	ss->Release();
	depthBuff->Release();
	depthBuffer->Release();
}

ID3D11RenderTargetView* Light::GetRTV()
{
	return renderTarget;
}

ID3D11ShaderResourceView* Light::GetSRV()
{
	return shaderResource;
}
