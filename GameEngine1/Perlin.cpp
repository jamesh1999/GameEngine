#include "Perlin.h"
#include <cmath>
#include <DirectXMath.h>
#include <iostream>

float dotGridPoint(float* vectors, int x, int y, float dx, float dy)
{
	return vectors[(x + y * x) * 2] * dx
		+ vectors[(x + y * x) * 2 + 1] * dy;
}

void Perlin::FillGradientVectors(float* out, int w, int h)
{
	for (int x = 0; x < w; ++x)
		for (int y = 0; y < h; ++y)
		{
			float theta = DirectX::XM_2PI * static_cast<float>(rand()) / RAND_MAX;
			out[(x + y * x) * 2] = sinf(theta);
			out[(x + y * x) * 2 + 1] = cosf(theta);
		}
}

void Perlin::GeneratePerlin(float* out, int w, int h, float weight, float frequency)
{
	int vec_w = static_cast<int>(ceil(w * frequency)) + 1;
	int vec_h = static_cast<int>(ceil(h * frequency)) + 1;

	float* vectors = new float[vec_w * vec_h * 2];
	FillGradientVectors(vectors, vec_w, vec_h);

	for (int x = 0; x < w; ++x)
		for (int y = 0; y < h; ++y)
		{
			int x0 = static_cast<int>(floor(x * frequency));
			int x1 = x0 + 1;
			int y0 = static_cast<int>(floor(y * frequency));
			int y1 = y0 + 1;

			float dx = static_cast<float>(x) * frequency - x0;
			float dy = static_cast<float>(y) * frequency - y0;

			float tot = 0.0f;
			tot += dotGridPoint(vectors, x0, y0, dx, dy) * (1 - dx) * (1 - dy);
			tot += dotGridPoint(vectors, x0, y1, dx, dy - 1) * (1 - dx) * dy;
			tot += dotGridPoint(vectors, x1, y0, dx - 1, dy) * dx * (1 - dy);
			tot += dotGridPoint(vectors, x1, y1, dx - 1, dy - 1) * dx * dy;

			out[x + y * w] += tot * weight;
		}

	delete[] vectors;
}
