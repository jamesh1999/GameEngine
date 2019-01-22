#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <tuple>
#include "Engine.h"
#include "ResourceRef.h"
#include "TextureArray.h"
#include "Resource.h"

class RenderPass
{
public:
	GameEngine::Engine* engine;
	ID3D11VertexShader* vs = nullptr;
	ID3D11PixelShader* ps = nullptr;
	ID3D11InputLayout* layout = nullptr;

	void LoadVS(std::wstring filename, std::string entry, D3D11_INPUT_ELEMENT_DESC* inputDesc, int numElements);
	void LoadPS(std::wstring filename, std::string entry);
};

class Material : public GameEngine::Resources::Resource
{

protected:
	Material* CloneResource();

public:
	std::vector<RenderPass> passes;

	ID3D11SamplerState* m_samplerState = nullptr;
	bool m_pushed = false;

	~Material();

	ID3D11SamplerState* GetSampler();
};

#endif
