#include "Mesh.h"
#include "ResourceFactory.h"

using namespace GameEngine::Resources;

Mesh* Mesh::CloneResource()
{
	Mesh* nMesh = engine->resourceFactory->Create<Mesh>("");

	nMesh->indices = indices;
	nMesh->vertices = vertices;

	return nMesh;
}

Mesh* Mesh::CombineMesh(Mesh* additional)
{
	Mesh* nMesh = CloneResource();

	// Append vertices and offseted indices
	int offset = vertices.size();
	for (Vertex v : additional->vertices)
		nMesh->vertices.push_back(v);
	for (int i : additional->indices)
		nMesh->indices.push_back(i + offset);

	return nMesh;
}

DirectX::XMVECTOR Mesh::GetCentroid()
{
	if (m_generated) return DirectX::XMLoadFloat3(&m_centroid);

	m_centroid = { 0.0f, 0.0f, 0.0f };
	for (Vertex v : vertices)
	{
		m_centroid.x += v.pos.x;
		m_centroid.y += v.pos.y;
		m_centroid.z += v.pos.z;
	}

	m_centroid.x /= vertices.size();
	m_centroid.y /= vertices.size();
	m_centroid.z /= vertices.size();

	m_generated = true;

	return DirectX::XMLoadFloat3(&m_centroid);
}