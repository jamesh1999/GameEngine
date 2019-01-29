#include "Geometry.h"
#include <DirectXMath.h>

void Geometry::GenerateCuboid(GameEngine::Resources::Mesh* mesh, float w, float h, float d)
{
	Vertex buff;

	//Positions
	for (int i = 0; i < 24; ++i)
	{
		buff.pos = {
			i & 1 ? -w : w,
			i & 2 ? -h : h,
			i & 4 ? -d : d
		};

		mesh->vertices.push_back(buff);
	}

	//Normals
	for (int i = 0; i < 8; ++i)
	{
		if (mesh->vertices[i].pos.x > 0.0f)
			mesh->vertices[i].normal = {1.0f, 0.0f, 0.0f};
		else
			mesh->vertices[i].normal = {-1.0f, 0.0f, 0.0f};
	}
	for (int i = 8; i < 16; ++i)
	{
		if (mesh->vertices[i].pos.y > 0.0f)
			mesh->vertices[i].normal = {0.0f, 1.0f, 0.0f};
		else
			mesh->vertices[i].normal = {0.0f, -1.0f, 0.0f};
	}
	for (int i = 16; i < 24; ++i)
	{
		if (mesh->vertices[i].pos.z > 0.0f)
			mesh->vertices[i].normal = {0.0f, 0.0f, 1.0f};
		else
			mesh->vertices[i].normal = {0.0f, 0.0f, -1.0f};
	}

	//Indices
	int indices[36] =
	{
		0,
		2,
		6,
		4,
		0,
		6,
		1,
		5,
		7,
		1,
		7,
		3,

		8,
		12,
		9,
		9,
		12,
		13,
		11,
		15,
		14,
		14,
		10,
		11,

		16,
		17,
		19,
		16,
		19,
		18,
		21,
		20,
		23,
		20,
		22,
		23,
	};
	for (int i : indices)
		mesh->indices.push_back(i);
}

void Geometry::GenerateSphere(GameEngine::Resources::Mesh* mesh, float radius, int nSlices, int nSegments)
{
	//Sides
	float deltaH = 2.0f / nSlices;
	float deltaTheta = DirectX::XM_2PI / nSegments;

	Vertex buff;
	float y = deltaH - 1;
	for (int i = 0; i < nSlices - 1; ++i)
	{
		float theta = 0.0f;
		for (int j = 0; j <= nSegments; ++j)
		{
			float xz_sf = sqrtf(1 - powf(y, 2.0));
			buff.normal = {sinf(theta) * xz_sf, y, cosf(theta) * xz_sf};

			buff.pos = {buff.normal.x * radius, buff.normal.y * radius, buff.normal.z * radius};

			mesh->vertices.push_back(buff);

			theta += deltaTheta;
		}
		y += deltaH;
	}

	//Side indices
	for (int i = 0; i < (nSlices - 1) * (nSegments + 1); ++i)
	{
		if (i % (nSegments + 1) == nSegments)
			continue;

		mesh->indices.push_back(i);
		mesh->indices.push_back(i + nSegments + 2);
		mesh->indices.push_back(i + nSegments + 1);

		mesh->indices.push_back(i + 1);
		mesh->indices.push_back(i + nSegments + 2);
		mesh->indices.push_back(i);
	}

	//Bottom cap
	buff.normal = {0, -1, 0};
	buff.pos = {0.0f, -radius, 0.0f};
	mesh->vertices.push_back(buff);

	//Bottom indices
	int baseIndex = (nSlices - 1) * (nSegments + 1);
	for (int i = 0; i < nSegments; ++i)
	{
		mesh->indices.push_back(i + 1);
		mesh->indices.push_back(i);
		mesh->indices.push_back(baseIndex);
	}

	//Top cap
	buff.normal = {0, 1, 0};
	buff.pos = {0.0f, radius, 0.0f};
	mesh->vertices.push_back(buff);

	//Top Indices
	baseIndex = (nSlices - 1) * (nSegments + 1) + 1;
	for (int i = (nSlices - 2) * (nSegments + 1); i < (nSlices - 2) * (nSegments + 1) + nSegments; ++i)
	{
		mesh->indices.push_back(i);
		mesh->indices.push_back(i + 1);
		mesh->indices.push_back(baseIndex);
	}
}

void Geometry::GenerateCylinder(GameEngine::Resources::Mesh* mesh, float topRadius, float bottomRadius, float h,
                                int nSlices, int nSegments)
{
	//Sides
	float deltaH = h / nSlices;
	float deltaTheta = DirectX::XM_2PI / nSegments;
	float deltaR = (topRadius - bottomRadius) / nSlices;

	Vertex buff;
	float y = -h / 2;
	float radius = bottomRadius;
	for (int i = 0; i <= nSlices; ++i)
	{
		float theta = 0.0f;
		for (int j = 0; j <= nSegments; ++j)
		{
			buff.normal = {sinf(theta), y, cosf(theta)};
			buff.pos = {sinf(theta) * radius, y, cosf(theta) * radius};
			buff.tex = {theta * DirectX::XM_1DIV2PI, 0.25f + y / (h * 2), 0.0f};

			mesh->vertices.push_back(buff);

			theta += deltaTheta;
		}

		radius += deltaR;
		y += deltaH;
	}

	//Side indices
	for (int i = 0; i < nSlices * (nSegments + 1); ++i)
	{
		if (i % (nSegments + 1) == nSegments)
			continue;

		mesh->indices.push_back(i);
		mesh->indices.push_back(i + nSegments + 2);
		mesh->indices.push_back(i + nSegments + 1);

		mesh->indices.push_back(i + 1);
		mesh->indices.push_back(i + nSegments + 2);
		mesh->indices.push_back(i);
	}

	//Bottom cap
	buff.normal = {0, -1, 0};
	float theta = 0.0f;
	for (int j = 0; j < nSegments; ++j)
	{
		buff.pos = {sinf(theta) * bottomRadius, -h / 2, cosf(theta) * bottomRadius};
		buff.tex = {sinf(theta) / 4 + 0.25f, cosf(theta) / 4 + 0.75f, 0.0f};

		mesh->vertices.push_back(buff);

		theta += deltaTheta;
	}

	//Bottom indices
	int baseIndex = (nSlices + 1) * (nSegments + 1);
	for (int i = baseIndex + 1; i < baseIndex + nSegments - 1; ++i)
	{
		mesh->indices.push_back(i + 1);
		mesh->indices.push_back(i);
		mesh->indices.push_back(baseIndex);
	}

	//Top cap
	buff.normal = {0, 1, 0};
	theta = 0.0f;
	for (int j = 0; j < nSegments; ++j)
	{
		buff.pos = {sinf(theta) * topRadius, h / 2, cosf(theta) * topRadius};
		buff.tex = {sinf(theta) / 4 + 0.75f, cosf(theta) / 4 + 0.75f, 0.0f};

		mesh->vertices.push_back(buff);

		theta += deltaTheta;
	}

	//Top Indices
	baseIndex = (nSlices + 1) * (nSegments + 1) + nSegments;
	for (int i = baseIndex + 1; i < baseIndex + nSegments - 1; ++i)
	{
		mesh->indices.push_back(i);
		mesh->indices.push_back(i + 1);
		mesh->indices.push_back(baseIndex);
	}
}

void Geometry::GenerateQuad(GameEngine::Resources::Mesh* mesh, float w, float l, int xSplit, int ySplit)
{
	//Vertices
	float origX = -w / 2;
	float origY = -l / 2;
	float deltaX = w / static_cast<float>(xSplit);
	float deltaY = l / static_cast<float>(ySplit);

	Vertex buff;
	buff.normal = {0.0f, 1.0f, 0.0f};
	for (int x = 0; x <= xSplit; ++x)
		for (int y = 0; y <= ySplit; ++y)
		{
			buff.pos = {origX + deltaX * x, 0.0f, origY + deltaY * y};
			buff.tex = {static_cast<float>(x) / xSplit, static_cast<float>(y) / ySplit, 0.0f};

			mesh->vertices.push_back(buff);
		}

	//Indices
	for (int i = 0; i < xSplit; ++i)
		for (int j = 0; j < ySplit; ++j)
		{
			mesh->indices.push_back(i * (ySplit + 1) + j);
			mesh->indices.push_back((i + 1) * (ySplit + 1) + j);
			mesh->indices.push_back((i + 1) * (ySplit + 1) + j + 1);

			mesh->indices.push_back(i * (ySplit + 1) + j);
			mesh->indices.push_back((i + 1) * (ySplit + 1) + j + 1);
			mesh->indices.push_back(i * (ySplit + 1) + j + 1);
		}
}
