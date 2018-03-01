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

class Material
{
	GameEngine::Resources::ResourceRef<GameEngine::Resources::TextureArray> m_textures;

	GameEngine::Engine* engine;

public:
	std::vector<RenderPass> passes;

	ID3D11SamplerState* m_samplerState = nullptr;

	Material(GameEngine::Engine* e) : engine(e) {};
	~Material();

	Material(const Material&);
	Material(Material&&) noexcept;
	Material& operator=(const Material&);
	Material& operator=(Material&&) noexcept;

	ID3D11ShaderResourceView* GetTexture() const;
	ID3D11SamplerState* GetSampler() const;
	void LoadTGA(std::string);
	void LoadTGAArray(std::vector<std::string>);
	void SetTexture(DirectX::XMFLOAT3*, int, int);
};

class MaterialGroup
{
private:
	std::vector<Material*> materials;

public:
	MaterialGroup() = default;
	~MaterialGroup();

	MaterialGroup(const MaterialGroup&);
	MaterialGroup(MaterialGroup&&) noexcept;
	MaterialGroup& operator=(const MaterialGroup&);
	MaterialGroup& operator=(MaterialGroup&&) noexcept;

	Material* operator[](int);
	int Size() const;

	void AddMaterial(Material*);
};

#endif
