#include "Texture.h"
#include <cmath>
#include "GraphicsController.h"

#define TEX_E 2.718281828459

using namespace GameEngine::Resources;

int Texture::GetWidth() const
{
	return m_w;
}

int Texture::GetHeight() const
{
	return m_h;
}

//Resize the texture using bilinear filtering
void Texture::Resize(int width, int height)
{
	float* new_data = new float[4 * width * height];

	//Bilinear filtering
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			//Position of new pixel on original texture
			float orig_x = (static_cast<float>(x) + 0.5f) / width * m_w - 0.5f;
			float orig_y = (static_cast<float>(y) + 0.5f) / height * m_h - 0.5f;

			//Original pixels to blend/coefficients
			int col_x0 = static_cast<int>(std::floorf(orig_x));
			int col_x1 = static_cast<int>(std::ceilf(orig_x));
			int col_y0 = static_cast<int>(std::floorf(orig_y));
			int col_y1 = static_cast<int>(std::ceilf(orig_y));

			float coeff_x0 = 1.0f - (orig_x - col_x0);
			float coeff_x1 = 1.0f - (col_x1 - orig_x);
			float coeff_y0 = 1.0f - (orig_y - col_y0);
			float coeff_y1 = 1.0f - (col_y1 - orig_y);

			//Edge pixels
			if (col_x0 < 0)
			{
				col_x0 = 0;
				coeff_x0 = 1.0f;
				coeff_x1 = 0.0f;
			}

			if (col_x1 >= m_w)
			{
				col_x1 = m_w - 1;
				coeff_x0 = 0.0f;
				coeff_x1 = 1.0f;
			}

			if (col_y0 < 0)
			{
				col_y0 = 0;
				coeff_y0 = 1.0f;
				coeff_y1 = 0.0f;
			}

			if (col_y1 >= m_h)
			{
				col_y1 = m_h - 1;
				coeff_y0 = 0.0f;
				coeff_y1 = 1.0f;
			}

			//Blending
			new_data[4 * (width * y + x) + 0] =
				coeff_x0 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x0) + 0]
				+ coeff_x1 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x1) + 0]
				+ coeff_x0 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x0) + 0]
				+ coeff_x1 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x1) + 0];
			new_data[4 * (width * y + x) + 1] =
				coeff_x0 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x0) + 1]
				+ coeff_x1 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x1) + 1]
				+ coeff_x0 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x0) + 1]
				+ coeff_x1 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x1) + 1];
			new_data[4 * (width * y + x) + 2] =
				coeff_x0 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x0) + 2]
				+ coeff_x1 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x1) + 2]
				+ coeff_x0 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x0) + 2]
				+ coeff_x1 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x1) + 2];
			new_data[4 * (width * y + x) + 3] =
				coeff_x0 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x0) + 3]
				+ coeff_x1 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x1) + 3]
				+ coeff_x0 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x0) + 3]
				+ coeff_x1 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x1) + 3];
		}

	delete m_data;
	m_data = new_data;
	m_w = width;
	m_h = height;
}

void Texture::Push()
{
	//Clear existing
	if (m_texture) m_texture->Release();
	if (m_SRV) m_SRV->Release();
	m_pushed = true;

	D3D11_TEXTURE2D_DESC tD;
	ZeroMemory(&tD, sizeof(D3D11_TEXTURE2D_DESC));
	tD.ArraySize = 1;
	tD.Usage = D3D11_USAGE_IMMUTABLE;
	tD.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tD.Height = m_h;
	tD.Width = m_w;
	tD.MiscFlags = 0;
	tD.MipLevels = 1;
	tD.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tD.SampleDesc.Count = 1;
	tD.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA texData;
	texData.SysMemSlicePitch = 4 * sizeof(float) * m_h * m_w;
	texData.SysMemPitch = 4 * sizeof(float) * m_w;
	texData.pSysMem = m_data;

	engine->graphics->device->CreateTexture2D(&tD, &texData, &m_texture);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvD;
	srvD.Format = tD.Format;
	srvD.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvD.Texture2D.MipLevels = -1;
	srvD.Texture2D.MostDetailedMip = 0;
	engine->graphics->device->CreateShaderResourceView(m_texture, NULL, &m_SRV);
}

ID3D11ShaderResourceView* Texture::GetSRV()
{
	if (!m_pushed) Push();
	return m_SRV;
}

void GaussianBlur(DirectX::XMFLOAT3** tex, int x, int y, float sigma)
{
	int radius = std::ceil(3 * sigma);

	float* coefficients = new float[(2 * radius + 1) * (2 * radius + 1)];
	double twoSdSquared = 2 * std::pow(sigma, 2.0);
	double reciprocalPiTwoSdSquared = 1.0 / (twoSdSquared * DirectX::XM_PI);
	for(int i = -radius; i <= radius; ++i)
		for(int j = -radius; j <= radius; ++j)
		{
			coefficients[(i + radius) * (2 * radius + 1) + (j + radius)] =
				reciprocalPiTwoSdSquared * std::pow(TEX_E,
					- (i * i + j * j) / twoSdSquared);
		}

	DirectX::XMFLOAT3* new_tex = new DirectX::XMFLOAT3[x * y];
	for(int i = 0; i < y; ++i)
		for(int j = 0; j < x; ++j)
		{
			new_tex[i * x + j] = { 0.0f, 0.0f, 0.0f };

			float tot = 0.0f;
			for (int k = -radius; k <= radius; ++k)
			{
				if (i + k < 0
					|| i + k >= y)
					continue;

				for (int l = -radius; l <= radius; ++l)
				{
					if (j + l < 0
						|| j + l >= x)
						continue;

					float coeff = coefficients[(k + radius) * (2 * radius + 1) + (l + radius)];
					tot += coeff;

					new_tex[i * x + j].x += (*tex)[(i + k) * x + (j + l)].x * coeff;
					new_tex[i * x + j].y += (*tex)[(i + k) * x + (j + l)].y * coeff;
					new_tex[i * x + j].z += (*tex)[(i + k) * x + (j + l)].z * coeff;
				}
			}

			new_tex[i * x + j].x /= tot;
			new_tex[i * x + j].y /= tot;
			new_tex[i * x + j].z /= tot;
		}

	delete[] *tex;

	*tex = new_tex;
}