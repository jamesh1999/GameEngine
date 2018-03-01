#include "TextureArray.h"
#include "GraphicsController.h"

using namespace GameEngine::Resources;

void TextureArray::Add(Texture* tex)
{
	m_textures.push_back(tex);
}

Texture* TextureArray::operator[](int idx)
{
	return m_textures[idx];
}

void TextureArray::Push()
{
	//Clear existing
	if (m_texture) m_texture->Release();
	if (m_SRV) m_SRV->Release();
	m_pushed = true;

	//Resize all textures to a common maximum size
	int max_width = -1;
	int max_height = -1;
	for (auto tex : m_textures)
	{
		if (tex->GetWidth() > max_width) max_width = tex->GetWidth();
		if (tex->GetHeight() > max_height) max_height = tex->GetHeight();
	}

	for (auto tex : m_textures)
	{
		if (tex->GetWidth() == max_width
			&& tex->GetHeight() == max_height) continue;

		tex->Resize(max_width, max_height);
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
	for (int i = 0; i < m_textures.size(); ++i)
	{
		texData[i].SysMemSlicePitch = 4 * sizeof(float) * max_height * max_width;
		texData[i].SysMemPitch = 4 * sizeof(float) * max_width;
		texData[i].pSysMem = m_textures[i]->m_data;
	}

	engine->graphics->device->CreateTexture2D(&tD, texData, &m_texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvD;
	srvD.Format = tD.Format;
	srvD.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvD.Texture2D.MipLevels = -1;
	srvD.Texture2D.MostDetailedMip = 0;
	engine->graphics->device->CreateShaderResourceView(m_texture, NULL, &m_SRV);
}

ID3D11ShaderResourceView* TextureArray::GetSRV()
{
	if (!m_pushed) Push();
	return m_SRV;
}