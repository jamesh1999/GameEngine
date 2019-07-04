#include "TextureArray.h"
#include <sstream>
#include "GraphicsController.h"
#include "ResourceFactory.h"

using namespace GameEngine::Resources;

TextureArray::~TextureArray()
{
	if (!m_pushed) return;
	m_texture->Release();
	m_SRV->Release();
}

TextureArray* TextureArray::CloneResource()
{
	TextureArray* nTA = GetEngine()->resourceFactory->Create<TextureArray>();

	nTA->m_textures = m_textures;

	return nTA;
}

TextureArray* TextureArray::Add(Texture* tex)
{
	std::stringstream ss;
	ss << '[';

	if (m_textures.size())
	{
		std::string identifier = GetIdentifier();
		ss << identifier.substr(1, identifier.size() - 2);
		ss << ';';
	}
	ss << tex->GetIdentifier();
	ss << ']';

	return GetEngine()->resourceFactory->Create<TextureArray>(ss.str());
}

Texture* TextureArray::operator[](int idx)
{
	return m_textures[idx].Get();
}

void TextureArray::Push()
{
	//Clear existing
	if (m_pushed)
	{
		m_texture->Release();
		m_SRV->Release();
	}
	m_pushed = true;

	//Resize all textures to a common maximum size
	int max_width = -1;
	int max_height = -1;
	for (auto tex : m_textures)
	{
		if (tex->GetWidth() > max_width) max_width = tex->GetWidth();
		if (tex->GetHeight() > max_height) max_height = tex->GetHeight();
	}

	for (auto& tex : m_textures)
	{
		if (tex->GetWidth() == max_width
			&& tex->GetHeight() == max_height)
			continue;

		tex = tex->Resize(max_width, max_height);

		//for (int x = 0; x < tex->GetWidth(); ++x)
		//for (int y = 0; y < tex->GetHeight(); ++y)
		//if (tex->m_data[(y * tex->GetWidth() + x) * 4 + 2] > 1.1f)
		//std::cout << "haha" << std::endl;
	}

	D3D11_TEXTURE2D_DESC tD;
	ZeroMemory(&tD, sizeof(D3D11_TEXTURE2D_DESC));
	tD.ArraySize = m_textures.size();
	tD.Usage = D3D11_USAGE_IMMUTABLE;
	tD.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tD.Height = max_height;
	tD.Width = max_width;
	tD.MiscFlags = 0;
	tD.MipLevels = 1;
	tD.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tD.SampleDesc.Count = 1;
	tD.SampleDesc.Quality = 0;

	//Populate with texture data
	D3D11_SUBRESOURCE_DATA* texData = new D3D11_SUBRESOURCE_DATA[m_textures.size()];
	for (size_t i = 0; i < m_textures.size(); ++i)
	{
		texData[i].SysMemSlicePitch = 4 * sizeof(float) * max_height * max_width;
		texData[i].SysMemPitch = 4 * sizeof(float) * max_width;
		texData[i].pSysMem = m_textures[i]->m_data;
	}

	GetEngine()->graphics->device->CreateTexture2D(&tD, texData, &m_texture);

	delete[] texData;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvD;
	srvD.Format = tD.Format;
	srvD.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvD.Texture2D.MipLevels = -1;
	srvD.Texture2D.MostDetailedMip = 0;
	GetEngine()->graphics->device->CreateShaderResourceView(m_texture, nullptr, &m_SRV);
}

ID3D11ShaderResourceView* TextureArray::GetSRV()
{
	if (!m_pushed) Push();
	return m_SRV;
}

bool TextureArray::GetTransparent() const
{
	return m_transparent;
}

int TextureArray::Find(Texture* t)
{
	for (size_t i = 0; i < m_textures.size(); ++i)
		if (m_textures[i].Get() == t) return i;
	return -1;
}

int TextureArray::Size() const
{
	return m_textures.size();
}
