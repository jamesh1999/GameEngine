#include "Material.h"
#include <fstream>
#include <cmath>
#include "GraphicsController.h"
#include <Windows.h>
#include <iostream>
#include "ResourceFactory.h"

void RenderPass::LoadVS(std::wstring filename, std::string entry, D3D11_INPUT_ELEMENT_DESC* inputDesc, int numElements)
{
	if (vs != nullptr)
		vs->Release();

	ID3D10Blob *buff = nullptr, *err = nullptr;
	HRESULT success = D3DCompileFromFile(
		filename.c_str(),
		NULL,
		NULL,
		entry.c_str(),
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		NULL,
		&buff,
		&err);

	if (success != S_OK)
	{
		std::cout << static_cast<char*>(err->GetBufferPointer()) << std::endl;
		system("pause");
		exit(-1);
	}
	else if (err != nullptr)
		err->Release();

	//Create shader and input layout
	engine->graphics->device->CreateVertexShader(
		buff->GetBufferPointer(), buff->GetBufferSize(), NULL, &vs);
	engine->graphics->device->CreateInputLayout(
		inputDesc, numElements, buff->GetBufferPointer(), buff->GetBufferSize(), &layout);
}

void RenderPass::LoadPS(std::wstring filename, std::string entry)
{
	if (ps != nullptr)
		ps->Release();

	ID3D10Blob *buff = nullptr, *err = nullptr;
	HRESULT success = D3DCompileFromFile(
		filename.c_str(),
		NULL,
		NULL,
		entry.c_str(),
		"ps_5_0",
		D3DCOMPILE_DEBUG,
		NULL,
		&buff,
		&err);

	if (success != S_OK)
	{
		std::cout << static_cast<char*>(err->GetBufferPointer()) << std::endl;
		system("pause");
		exit(-1);
	}
	else if (err != nullptr)
		err->Release();

	//Create shader
	engine->graphics->device->CreatePixelShader(
		buff->GetBufferPointer(), buff->GetBufferSize(), NULL, &ps);
}

Material::~Material()
{
	for(int i = 0; i < passes.size(); ++i)
	{
		passes[i].vs->Release();
		passes[i].ps->Release();
		passes[i].layout->Release();
	}

	passes.clear();

	m_samplerState->Release();
}

Material::Material(const Material& other)
{
	passes = other.passes;

	for(int i = 0; i < passes.size(); ++i)
	{
		passes[i].layout->AddRef();
		passes[i].ps->AddRef();
		passes[i].vs->AddRef();
	}

	m_textures = other.m_textures;

	m_samplerState = other.m_samplerState;
	m_samplerState->AddRef();
	engine = other.engine;
}

Material::Material(Material&& other) noexcept
{
	passes = other.passes;
	other.passes.clear();

	m_textures = other.m_textures;

	m_samplerState = other.m_samplerState;
	other.m_samplerState = nullptr;
	engine = other.engine;
}

Material& Material::operator=(const Material& other)
{
	passes = other.passes;

	for (int i = 0; i < passes.size(); ++i)
	{
		passes[i].layout->AddRef();
		passes[i].ps->AddRef();
		passes[i].vs->AddRef();
	}

	m_textures = other.m_textures;

	m_samplerState = other.m_samplerState;
	m_samplerState->AddRef();
	engine = other.engine;

	return *this;
}

Material& Material::operator=(Material&& other) noexcept
{
	passes = other.passes;
	other.passes.clear();

	m_textures = other.m_textures;

	m_samplerState = other.m_samplerState;
	other.m_samplerState = nullptr;
	engine = other.engine;

	return *this;
}

ID3D11ShaderResourceView* Material::GetTexture() const
{
	return m_textures->GetSRV();
}

ID3D11SamplerState* Material::GetSampler() const
{
	return m_samplerState;
}

void Material::LoadTGA(std::string filename)
{
	m_textures = engine->resourceFactory->Create<GameEngine::Resources::TextureArray>("");
	m_textures->Add(engine->resourceFactory->Create<GameEngine::Resources::Texture>(filename));

	D3D11_SAMPLER_DESC sD;
	sD.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sD.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sD.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sD.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sD.MipLODBias = 0.0f;
	sD.MaxAnisotropy = 1;
	sD.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sD.BorderColor[0] = 0;
	sD.BorderColor[1] = 0;
	sD.BorderColor[2] = 0;
	sD.BorderColor[3] = 0;
	sD.MinLOD = 0;
	sD.MaxLOD = D3D11_FLOAT32_MAX;

	engine->graphics->device->CreateSamplerState(&sD, &m_samplerState);
}

void Material::LoadTGAArray(std::vector<std::string> paths)
{
	m_textures = engine->resourceFactory->Create<GameEngine::Resources::TextureArray>("");
	for (auto path : paths)
	{
		m_textures->Add(engine->resourceFactory->Create<GameEngine::Resources::Texture>(path));
	}

	D3D11_SAMPLER_DESC sD;
	sD.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	sD.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sD.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sD.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sD.MipLODBias = 0.0f;
	sD.MaxAnisotropy = 1;
	sD.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sD.BorderColor[0] = 0;
	sD.BorderColor[1] = 0;
	sD.BorderColor[2] = 0;
	sD.BorderColor[3] = 0;
	sD.MinLOD = 0;
	sD.MaxLOD = D3D11_FLOAT32_MAX;

	engine->graphics->device->CreateSamplerState(&sD, &m_samplerState);
}


void Material::SetTexture(DirectX::XMFLOAT3* data, int x, int y)
{
	uint8_t* textureData = new uint8_t[x * y * 4];

	for(int i = 0; i < y; ++i)
		for(int j = 0; j < x; ++j)
		{
			textureData[(i * x + j) * 4]     = std::round(data[i * x + j].x * 255);
			textureData[(i * x + j) * 4 + 1] = std::round(data[i * x + j].y * 255);
			textureData[(i * x + j) * 4 + 2] = std::round(data[i * x + j].z * 255);
			textureData[(i * x + j) * 4 + 3] = 1.0f;
		}

	//setTextureData(textureData, x, y);
}

MaterialGroup::~MaterialGroup()
{
	for (int i = 0; i < materials.size(); ++i)
		delete materials[i];
}

MaterialGroup::MaterialGroup(const MaterialGroup& other)
{
	for(Material* m : other.materials)
	{
		Material* nMat = new Material(*m);
		materials.push_back(nMat);
	}
}

MaterialGroup::MaterialGroup(MaterialGroup&& other) noexcept
{
	materials = other.materials;
	other.materials.clear();
}

MaterialGroup& MaterialGroup::operator=(const MaterialGroup& other)
{
	for (Material* m : other.materials)
	{
		Material* nMat = new Material(*m);
		materials.push_back(nMat);
	}
	
	return *this;
}

MaterialGroup& MaterialGroup::operator=(MaterialGroup&& other) noexcept
{
	materials = other.materials;
	other.materials.clear();

	return *this;
}

Material* MaterialGroup::operator[](int index)
{
	return materials[index];
}

int MaterialGroup::Size() const
{
	return materials.size();
}

void MaterialGroup::AddMaterial(Material* m)
{
	materials.push_back(m);
}