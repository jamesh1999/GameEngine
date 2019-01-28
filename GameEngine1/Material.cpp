#include "Material.h"
#include <fstream>
#include <cmath>
#include "GraphicsController.h"
#include <Windows.h>
#include <iostream>

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
	GraphicsController::instance->device->CreateVertexShader(
		buff->GetBufferPointer(), buff->GetBufferSize(), NULL, &vs);
	GraphicsController::instance->device->CreateInputLayout(
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
	GraphicsController::instance->device->CreatePixelShader(
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

	//No assigned texture
	if (m_tex == nullptr) return;

	m_tex->Release();
	m_texView->Release();
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

	//No assigned texture
	if (other.m_tex == nullptr) return;

	m_tex = other.m_tex;
	m_tex->AddRef();
	m_texView = other.m_texView;
	m_texView->AddRef();
	m_samplerState = other.m_samplerState;
	m_samplerState->AddRef();
}

Material::Material(Material&& other) noexcept
{
	passes = other.passes;
	other.passes.clear();

	m_tex = other.m_tex;
	other.m_tex = nullptr;
	m_texView = other.m_texView;
	other.m_texView = nullptr;
	m_samplerState = other.m_samplerState;
	other.m_samplerState = nullptr;
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

	//No assigned texture
	if (other.m_tex == nullptr) return *this;

	m_tex = other.m_tex;
	m_tex->AddRef();
	m_texView = other.m_texView;
	m_texView->AddRef();
	m_samplerState = other.m_samplerState;
	m_samplerState->AddRef();

	return *this;
}

Material& Material::operator=(Material&& other) noexcept
{
	passes = other.passes;
	other.passes.clear();

	m_tex = other.m_tex;
	other.m_tex = nullptr;
	m_texView = other.m_texView;
	other.m_texView = nullptr;
	m_samplerState = other.m_samplerState;
	other.m_samplerState = nullptr;

	return *this;
}

ID3D11ShaderResourceView* Material::GetTexture() const
{
	return m_texView;
}

ID3D11SamplerState* Material::GetSampler() const
{
	return m_samplerState;
}

void Material::LoadTGA(std::string filename)
{
	auto dat = getTGAData(filename);
	setTextureData(std::get<0>(dat), std::get<1>(dat), std::get<2>(dat));
}

void Material::LoadTGAArray(std::vector<std::string> paths)
{
	std::vector<TexData> textures;

	for(std::string filename : paths)
	{
		auto dat = getTGAData(filename);
		textures.push_back({ std::get<0>(dat), std::get<1>(dat), std::get<2>(dat) });
	}

	setTextureData(textures);
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

	setTextureData(textureData, x, y);
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

void Material::setTextureData(uint8_t* data, int w, int h)
{
	std::vector<TexData> vec;
	vec.push_back({ data, w, h });
	setTextureData(vec);
}

void texcpy(void* dest, void* src, int size, int yscale, int xscale, int nrows, int ncols)//, int row_offset, int col_offset)
{
	uint8_t* udest = static_cast<uint8_t*>(dest);
	uint8_t* usrc = static_cast<uint8_t*>(src);

	/*for(int i = 0; i < nrows; ++i)
	{
		memcpy(udest + (i + row_offset) * ncols * size + col_offset * size, usrc + ncols * i * size, ncols * size);
	}*/

	for(int i = 0; i < nrows; ++i)
		for(int j = 0; j < ncols; ++j)
		{
			udest[(i * ncols + j) * size] = usrc[((i / yscale) * (ncols / yscale) + j / xscale) * size];
			udest[(i * ncols + j) * size + 1] = usrc[((i / yscale) * (ncols / yscale) + j / xscale) * size + 1];
			udest[(i * ncols + j) * size + 2] = usrc[((i / yscale) * (ncols / yscale) + j / xscale) * size + 2];
			udest[(i * ncols + j) * size + 3] = usrc[((i / yscale) * (ncols / yscale) + j / xscale) * size + 3];
		}
}

void Material::setTextureData(std::vector<TexData> textures)
{
	if(m_tex != nullptr)
	{
		m_tex->Release();
		m_texView->Release();
		m_samplerState->Release();
	}

	int w_max = INT_MIN;
	int h_max = INT_MIN;
	for(TexData tex : textures)
	{
		if (tex.h > h_max)
			h_max = tex.h;
		if (tex.w > w_max)
			w_max = tex.w;
	}

	for (TexData& tex : textures)
	{
		uint8_t* n_data = new uint8_t[w_max * h_max * 4];
		texcpy(n_data, tex.data, 4, h_max / tex.h, w_max / tex.w, h_max, w_max);

		delete[] tex.data;
		tex.data = n_data;
	}



	D3D11_TEXTURE2D_DESC tD;
	ZeroMemory(&tD, sizeof(D3D11_TEXTURE2D_DESC));
	tD.Usage = D3D11_USAGE_IMMUTABLE;
	tD.ArraySize = textures.size();
	tD.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tD.Height = h_max;
	tD.Width = w_max;
	tD.MiscFlags = 0;
	tD.MipLevels = 1;
	tD.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tD.SampleDesc.Count = 1;
	tD.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA* texData = new D3D11_SUBRESOURCE_DATA[textures.size()];
	for(int i = 0; i < textures.size(); ++i)
	{
		texData[i].SysMemSlicePitch = 4 * w_max * h_max;
		texData[i].SysMemPitch = 4 * w_max;
		texData[i].pSysMem = textures[i].data;
	}

	GraphicsController::instance->device->CreateTexture2D(&tD, texData, &m_tex);



	D3D11_SHADER_RESOURCE_VIEW_DESC srvD;
	srvD.Format = tD.Format;
	srvD.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvD.Texture2D.MipLevels = -1;
	srvD.Texture2D.MostDetailedMip = 0;
	GraphicsController::instance->device->CreateShaderResourceView(m_tex, NULL, &m_texView);
	//GraphicsController::instance->devContext->GenerateMips(m_texView);

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

	GraphicsController::instance->device->CreateSamplerState(&sD, &m_samplerState);

	for (TexData tex : textures)
		delete[] tex.data;
}

std::tuple<uint8_t*, int, int> Material::getTGAData(std::string filename)
{
	std::ifstream in(filename, std::ios::binary);

	TGAHeader h;
	in.read(reinterpret_cast<char*>(&h), sizeof(TGAHeader));

	uint8_t* tgadata;
	//32 bits per pixel
	if (h.bpp == 32)
	{
		tgadata = new uint8_t[h.width * h.height * 4];
		in.read(reinterpret_cast<char*>(tgadata), h.width * h.height * 4);
	}
	//24 bits per pixel
	else if (h.bpp == 24)
	{
		tgadata = new uint8_t[h.width * h.height * 3];
		in.read(reinterpret_cast<char*>(tgadata), h.width * h.height * 3);
	}
	else
		exit(-1);
	in.close();

	uint8_t* textureData = new uint8_t[h.width * h.height * 4];
	for (int i = 0; i < h.height; ++i)
	{
		for (int j = 0; j < h.width; ++j)
		{
			if (h.bpp == 32)
			{
				textureData[(i * h.width + j) * 4] = tgadata[((h.height - i - 1) * h.width + j) * 4 + 2];
				textureData[(i * h.width + j) * 4 + 1] = tgadata[((h.height - i - 1) * h.width + j) * 4 + 1];
				textureData[(i * h.width + j) * 4 + 2] = tgadata[((h.height - i - 1) * h.width + j) * 4];
				textureData[(i * h.width + j) * 4 + 3] = tgadata[((h.height - i - 1) * h.width + j) * 4 + 3];
			}
			else if (h.bpp == 24)
			{
				textureData[(i * h.width + j) * 4] = tgadata[((h.height - i - 1) * h.width + j) * 3 + 2];
				textureData[(i * h.width + j) * 4 + 1] = tgadata[((h.height - i - 1) * h.width + j) * 3 + 1];
				textureData[(i * h.width + j) * 4 + 2] = tgadata[((h.height - i - 1) * h.width + j) * 3];
				textureData[(i * h.width + j) * 4 + 3] = 255;
			}
		}
	}
	delete[] tgadata;

	return std::tuple<uint8_t*, int, int>(textureData, h.width, h.height);
}
