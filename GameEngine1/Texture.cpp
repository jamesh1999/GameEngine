#include "Texture.h"
#include <cmath>
#include "GraphicsController.h"
#include "ResourceFactory.h"

#define TEX_E 2.718281828459

using namespace GameEngine::Resources;

Texture::~Texture()
{
	if (m_data == nullptr) return;
	delete[] m_data;
}

Texture* Texture::CloneResource()
{
	Texture* nTex = engine->resourceFactory->Create<Texture>();

	nTex->m_h = m_h;
	nTex->m_w = m_w;
	nTex->m_opaque = m_opaque;
	nTex->m_data = new float[m_h * m_w * 4];
	memcpy(m_data, nTex->m_data, m_h * m_w * 4 * sizeof(float));

	return nTex;
}

int Texture::GetWidth() const
{
	return m_w;
}

int Texture::GetHeight() const
{
	return m_h;
}

bool Texture::IsOpaque() const
{
	return m_opaque;
}

//Resize the texture using bilinear filtering
Texture* Texture::Resize(int width, int height)
{
	Texture* nTex = engine->resourceFactory->Create<Texture>();

	nTex->m_h = width;
	nTex->m_w = height;
	nTex->m_opaque = m_opaque;

	nTex->m_data = new float[4 * width * height];

	//Bilinear filtering
	for (int y = 0; y < height; ++y)
		for (int x = 0; x < width; ++x)
		{
			//Position of new pixel on original texture
			float orig_x = (static_cast<float>(x) + 0.5f) / width * m_w - 0.5f;
			float orig_y = (static_cast<float>(y) + 0.5f) / height * m_h - 0.5f;

			//Original pixels to blend/coefficients
			int col_x0 = static_cast<int>(floorf(orig_x));
			int col_x1 = static_cast<int>(ceilf(orig_x));
			int col_y0 = static_cast<int>(floorf(orig_y));
			int col_y1 = static_cast<int>(ceilf(orig_y));

			float coeff_x0 = 1.0f - (orig_x - col_x0);
			float coeff_x1 = 1.0f - coeff_x0; //1.0f - (col_x1 - orig_x);
			float coeff_y0 = 1.0f - (orig_y - col_y0);
			float coeff_y1 = 1.0f - coeff_y0; //1.0f - (col_y1 - orig_y);

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
			nTex->m_data[4 * (width * y + x) + 0] =
				coeff_x0 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x0) + 0]
				+ coeff_x1 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x1) + 0]
				+ coeff_x0 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x0) + 0]
				+ coeff_x1 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x1) + 0];
			nTex->m_data[4 * (width * y + x) + 1] =
				coeff_x0 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x0) + 1]
				+ coeff_x1 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x1) + 1]
				+ coeff_x0 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x0) + 1]
				+ coeff_x1 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x1) + 1];
			nTex->m_data[4 * (width * y + x) + 2] =
				coeff_x0 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x0) + 2]
				+ coeff_x1 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x1) + 2]
				+ coeff_x0 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x0) + 2]
				+ coeff_x1 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x1) + 2];
			nTex->m_data[4 * (width * y + x) + 3] =
				coeff_x0 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x0) + 3]
				+ coeff_x1 * coeff_y0 * m_data[4 * (m_w * col_y0 + col_x1) + 3]
				+ coeff_x0 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x0) + 3]
				+ coeff_x1 * coeff_y1 * m_data[4 * (m_w * col_y1 + col_x1) + 3];
		}

	return nTex;
}

void GaussianBlur(DirectX::XMFLOAT3** tex, int x, int y, float sigma)
{
	int radius = static_cast<int>(ceil(3 * sigma));

	float* coefficients = new float[(2 * radius + 1) * (2 * radius + 1)];
	double twoSdSquared = 2 * pow(sigma, 2.0);
	double reciprocalPiTwoSdSquared = 1.0 / (twoSdSquared * DirectX::XM_PI);
	for (int i = -radius; i <= radius; ++i)
		for (int j = -radius; j <= radius; ++j)
		{
			coefficients[(i + radius) * (2 * radius + 1) + (j + radius)] =
				static_cast<float>(reciprocalPiTwoSdSquared * pow(TEX_E,
				                                                  - (i * i + j * j) / twoSdSquared));
		}

	DirectX::XMFLOAT3* new_tex = new DirectX::XMFLOAT3[x * y];
	for (int i = 0; i < y; ++i)
		for (int j = 0; j < x; ++j)
		{
			new_tex[i * x + j] = {0.0f, 0.0f, 0.0f};

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
	delete[] coefficients;

	*tex = new_tex;
}
