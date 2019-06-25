#include "Mesh.h"
#include <istream>
#include <ostream>
#include "ResourceFactory.h"

using namespace GameEngine::Resources;

Vertex& Vertex::operator<<(std::istream& in)
{
	in >> pos.x >> pos.y >> pos.z;
	in >> normal.x >> normal.y >> normal.z;
	in >> tex.x >> tex.y >> tex.z;

	return *this;
}

Vertex& Vertex::operator>>(std::ostream& out)
{
	out << pos.x << ' ' << pos.y << ' ' << pos.z << '\n';
	out << normal.x << ' ' << normal.y << ' ' << normal.z << '\n';
	out << tex.x << ' ' << tex.y << ' ' << tex.z << '\n';

	return *this;
}

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
	if (m_generated) return XMLoadFloat3(&m_centroid);

	m_centroid = {0.0f, 0.0f, 0.0f};
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

	return XMLoadFloat3(&m_centroid);
}

Mesh& Mesh::operator<<(std::istream& in)
{
	// Chain call base
	Resource::operator<<(in);

	int vtxCount, idxCount;
	in >> vtxCount >> idxCount;

	vertices.resize(vtxCount);
	indices.resize(idxCount);

	for (int i = 0; i < vtxCount; ++i)
		vertices[i] << in;

	for (int i = 0; i < idxCount; ++i)
		in >> indices[i];

	return *this;
}

Mesh& Mesh::operator>>(std::ostream& out)
{
	// Chain call base
	Resource::operator>>(out);

	out << vertices.size() << ' ' << indices.size() << '\n';

	for (Vertex& vtx : vertices)
		vtx >> out;

	for (int idx : indices)
		out << idx << '\n';

	return *this;
}