#include "Terrain.h"
#include "Texture.h"
#include "FractalFault.h"

void Terrain::ApplyHeightmap(GameEngine::Resources::Mesh* mesh)
{
	float* perlinTex = new float[101 * 101];
	for (int i = 0; i < 101 * 101; ++i)
		perlinTex[i] = 0.0f;

	Perlin::GeneratePerlin(perlinTex, 101, 101, 3.0f, 0.0002);
	Perlin::GeneratePerlin(perlinTex, 101, 101, 1.4f, 0.0004);
	Perlin::GeneratePerlin(perlinTex, 101, 101, 0.4f, 0.006);
	Perlin::GeneratePerlin(perlinTex, 101, 101, 0.1f, 0.012);
	Generation::FractalFault::Generate(perlinTex, 101, 101, 140000);

	ThermalErosion(perlinTex, 101, 101, 40, 0.007f);


	//Apply heights
	for(int i = 0; i < 101 * 101; ++i)
	{
		mesh->vertices[i].pos.y = perlinTex[i] * 6.0f;
	}

	//Correct normals
	for(int x = 1; x < 100; ++x)
		for(int y = 1; y < 100; ++y)
		{
			DirectX::XMVECTOR x0 = DirectX::XMLoadFloat3(&mesh->vertices[y + (x - 1) * 101].pos);
			DirectX::XMVECTOR x1 = DirectX::XMLoadFloat3(&mesh->vertices[y + (x + 1) * 101].pos);
			DirectX::XMVECTOR y0 = DirectX::XMLoadFloat3(&mesh->vertices[y - 1 + x * 101].pos);
			DirectX::XMVECTOR y1 = DirectX::XMLoadFloat3(&mesh->vertices[y + 1 + x * 101].pos);

			DirectX::XMVECTOR dX = DirectX::XMVectorSubtract(x1, x0);
			DirectX::XMVECTOR dY = DirectX::XMVectorSubtract(y1, y0);

			DirectX::XMStoreFloat3(&mesh->vertices[y + x * 101].normal,
				DirectX::XMVector3Normalize(DirectX::XMVector3Cross(dX, dY)));
		}

	delete[] perlinTex;
}

void Terrain::GenerateGroundTexture(DirectX::XMFLOAT3** out, int x, int y, GameEngine::Resources::Mesh* in)
{
	//Create perlin noise
	float* noiseTex = new float[x * x];
	for (int i = 0; i < x * y; ++i)
		noiseTex[i] = 0.0f;

	Perlin::GeneratePerlin(noiseTex, x, y, 1.2f, 0.01);
	Perlin::GeneratePerlin(noiseTex, x, y, 0.5f, 0.2);

	//Calculate width/height of input Quad
	float max = -FLT_MAX;
	int quad_z = 0;
	while (in->vertices[quad_z].pos.z > max)
		max = in->vertices[quad_z++].pos.z;
	int quad_x = in->vertices.size() / quad_z;

	double block_x = 1.0 / (quad_x - 1);
	double block_z = 1.0 / (quad_z - 1);

	//Base grass/dirt colours
	DirectX::XMFLOAT3 green = { 0.1f, 0.5f, 0.03f };
	DirectX::XMFLOAT3 brown = { 0.65f, 0.3f, 0.1f };

	for(Vertex& v : in->vertices)
	{
		DirectX::XMFLOAT3 colour =
			v.normal.y < -0.8
			? green
			: brown;

		int max_x = std::round((v.tex.x + block_x / 2) * x);
		int max_y = std::round((v.tex.y + block_z / 2) * y);

		for (int cur_x = std::round((v.tex.x - block_x / 2) * x); cur_x < max_x; ++cur_x)
		{
			if (cur_x < 0
				|| cur_x >= x)
				continue;;

			for (int cur_y = std::round((v.tex.y - block_z / 2) * y); cur_y < max_y; ++cur_y)
			{
				if (cur_y < 0
					|| cur_y >= y)
					continue;;

				(*out)[cur_y * x + cur_x].x = colour.x;
				(*out)[cur_y * x + cur_x].y = colour.y;
				(*out)[cur_y * x + cur_x].z = colour.z;
			}
		}
	}

	//Texture::GaussianBlur(out, x, y, 4.0f);

	for(int i = 0; i < x * y; ++i)
	{
		(*out)[i].x += noiseTex[i] * 0.2f;
		if ((*out)[i].x < 0.0f)
			(*out)[i].x = 0.0f;
		else if ((*out)[i].x > 1.0f)
			(*out)[i].x = 1.0f;

		(*out)[i].y += noiseTex[i] * 0.2f;
		if ((*out)[i].y < 0.0f)
			(*out)[i].y = 0.0f;
		else if ((*out)[i].y > 1.0f)
			(*out)[i].y = 1.0f;

		(*out)[i].z += noiseTex[i] * 0.2f;
		if ((*out)[i].z < 0.0f)
			(*out)[i].z = 0.0f;
		else if ((*out)[i].z > 1.0f)
			(*out)[i].z = 1.0f;
	}
}

void Terrain::ThermalErosion(float* heightmap, int w, int h, int n, float amount)
{
	for(int i = 0; i < n; ++i)
	{
		for(int y = 0; y < h; ++y)
			for(int x = 0; x < w; ++x)
			{
				//Apply thermal erosion in x and y

				//X
				if(x > 0)
				{
					float delta = heightmap[y * w + x] - heightmap[y * w + x - 1];
					heightmap[y * w + x - 1] += delta * amount;
					heightmap[y * w + x] -= delta * amount;
				}

				//Y
				if (y > 0)
				{
					float delta = heightmap[y * w + x] - heightmap[(y - 1) * w + x];
					heightmap[(y - 1) * w + x] += delta * amount;
					heightmap[y * w + x] -= delta * amount;
				}
			}
	}
}