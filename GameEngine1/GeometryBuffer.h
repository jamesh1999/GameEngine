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

	bool m_pushed = false;

	ID3D11Buffer* vertexBuffer = nullptr;
	ID3D11Buffer* indexBuffer = nullptr;

	unsigned vertexPos = 0;
	unsigned indexPos = 0;
	unsigned vertexSize = 0;
	unsigned indexSize = 0;

	std::vector<GameEngine::Renderer*> m_renderers;

	ID3D11Device* dev = nullptr;
	ID3D11DeviceContext* devContext = nullptr;

	void Push();

public:

	GeometryBuffer(ID3D11Device*, ID3D11DeviceContext*, bool);
	GeometryBuffer(ID3D11Device* d, ID3D11DeviceContext* dc) : GeometryBuffer(d, dc, false) {};
	~GeometryBuffer();

	GeometryBuffer(const GeometryBuffer&) = delete;
	GeometryBuffer& operator=(const GeometryBuffer&) = delete;
	GeometryBuffer(GeometryBuffer&& other)
	{
		if (vertexBuffer) vertexBuffer->Release();
		if (indexBuffer) indexBuffer->Release();
		vertexBuffer = other.vertexBuffer;
		other.vertexBuffer = nullptr;
		indexBuffer = other.indexBuffer;
		other.indexBuffer = nullptr;

		vertexPos = other.vertexPos;
		indexPos = other.indexPos;
		vertexSize = other.vertexSize;
		indexSize = other.indexSize;

		m_static = other.m_static;
		m_renderers = other.m_renderers;

		//if (dev) dev->Release();
		//if (devContext) devContext->Release();
		dev = other.dev;
		devContext = other.devContext;
		dev->AddRef();
		devContext->AddRef();
	};
	GeometryBuffer& operator=(GeometryBuffer&& other)
	{
		if (vertexBuffer) vertexBuffer->Release();
		if (indexBuffer) indexBuffer->Release();
		vertexBuffer = other.vertexBuffer;
		other.vertexBuffer = nullptr;
		indexBuffer = other.indexBuffer;
		other.indexBuffer = nullptr;

		vertexPos = other.vertexPos;
		indexPos = other.indexPos;
		vertexSize = other.vertexSize;
		indexSize = other.indexSize;

		m_static = other.m_static;
		m_renderers = other.m_renderers;

		//if (dev) dev->Release();
		//if (devContext) devContext->Release();
		dev = other.dev;
		devContext = other.devContext;
		dev->AddRef();
		devContext->AddRef();
	};

	void Resize(int, int);
	BufferLocation AddRenderer(GameEngine::Renderer*);
	void Select();
};

#endif