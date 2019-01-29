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
		nullptr,
		nullptr,
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
	if (err != nullptr)
		err->Release();

	//Create shader and input layout
	engine->graphics->device->CreateVertexShader(
		buff->GetBufferPointer(), buff->GetBufferSize(), nullptr, &vs);
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
		nullptr,
		nullptr,
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
	if (err != nullptr)
		err->Release();

	//Create shader
	engine->graphics->device->CreatePixelShader(
		buff->GetBufferPointer(), buff->GetBufferSize(), nullptr, &ps);
}

Material* Material::CloneResource()
{
	Material* nMaterial = engine->resourceFactory->Create<Material>("");
	nMaterial->passes = this->passes;
	//Todo: Actually do this properly ^^ (copy DX rsources)

	return nMaterial;
}

Material::~Material()
{
	for (size_t i = 0; i < passes.size(); ++i)
	{
		passes[i].vs->Release();
		passes[i].ps->Release();
		passes[i].layout->Release();
	}

	passes.clear();

	if (m_pushed) m_samplerState->Release();
}

ID3D11SamplerState* Material::GetSampler()
{
	if (m_pushed) return m_samplerState;

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

	m_pushed = true;

	return m_samplerState;
}
