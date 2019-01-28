#ifndef __LIGHT_INCLUDED__
#define __LIGHT_INCLUDED__

#include <d3d11.h>
#include "Component.h"

class Light : public Component
{
	ID3D11Texture2D* depthTex;
	ID3D11RenderTargetView* renderTarget;
	ID3D11ShaderResourceView* shaderResource;

public:
	ID3D11DepthStencilView* depthBuff;
	D3D11_VIEWPORT vp;
	ID3D11SamplerState* ss;
	Light();
	~Light();

	ID3D11RenderTargetView* GetRTV();
	ID3D11ShaderResourceView* GetSRV();
};

#endif