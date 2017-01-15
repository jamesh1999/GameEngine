#ifndef __MESHDATA_H__
#define __MESHDATA_H__

#include <DirectXMath.h>
#include <vector>

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tex;
};

struct SubMesh
{
	std::vector<Vertex> vertices;
	std::vector<int> indices;
};

typedef std::vector<SubMesh> MeshData;

void CombineMesh(SubMesh*, SubMesh*);

#endif