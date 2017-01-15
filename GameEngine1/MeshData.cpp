#include "MeshData.h"

void CombineMesh(SubMesh* base, SubMesh* additional)
{
	int offset = base->vertices.size();

	for (Vertex v : additional->vertices)
		base->vertices.push_back(v);
	for (int i : additional->indices)
		base->indices.push_back(i + offset);
}