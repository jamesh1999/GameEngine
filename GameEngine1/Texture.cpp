#include "Texture.h"
#include <cmath>

#define TEX_E 2.718281828459

void Texture::GaussianBlur(DirectX::XMFLOAT3** tex, int x, int y, float sigma)
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