#ifndef __GEOMETRY_BUFFER_INCLUDED__
#define __GEOMETRY_BUFFER_INCLUDED__

#include <d3d11.h>
#include <vector>
#include <tuple>
#include "Renderer.h"

class GeometryBuffer
{
public:

	using BufferLocation = std::tuple<unsigned, unsigned>;

private:

	bool m_static;

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	unsigned vertexPos = 0;
	unsigned indexPos = 0;
	unsigned vertexSize = 0;
	unsigned indexSize = 0;

	std::vector<int> indexBack;
	std::vector<Vertex> vertexBack;

public:

	GeometryBuffer(bool);
	GeometryBuffer() : GeometryBuffer(false) {};
	~GeometryBuffer();

	GeometryBuffer(const GeometryBuffer&) = delete;
	GeometryBuffer& operator=(const GeometryBuffer&) = delete;
	GeometryBuffer(GeometryBuffer&& other)
	{
		vertexBuffer = other.vertexBuffer;
		other.vertexBuffer = nullptr;
		indexBuffer = other.indexBuffer;
		other.indexBuffer = nullptr;

		vertexPos = other.vertexPos;
		indexPos = other.indexPos;
		vertexSize = other.vertexSize;
		indexSize = other.indexSize;

		m_static = other.m_static;
		indexBack = other.indexBack;
		vertexBack = other.vertexBack;
	};
	GeometryBuffer& operator=(GeometryBuffer&& other)
	{
		vertexBuffer = other.vertexBuffer;
		other.vertexBuffer = nullptr;
		indexBuffer = other.indexBuffer;
		other.indexBuffer = nullptr;

		vertexPos = other.vertexPos;
		indexPos = other.indexPos;
		vertexSize = other.vertexSize;
		indexSize = other.indexSize;

		m_static = other.m_static;
		indexBack = other.indexBack;
		vertexBack = other.vertexBack;
	};

	void Resize(int, int);
	BufferLocation AddRenderer(Renderer*);
	void Select();
};

#endif