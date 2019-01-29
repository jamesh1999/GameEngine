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

	void Resize(unsigned, unsigned);
	void Push();

public:

	GeometryBuffer(ID3D11Device*, ID3D11DeviceContext*, bool);
	GeometryBuffer(ID3D11Device* d, ID3D11DeviceContext* dc) : GeometryBuffer(d, dc, false) {};
	~GeometryBuffer();

	GeometryBuffer(const GeometryBuffer&) = delete;
	GeometryBuffer& operator=(const GeometryBuffer&) = delete;
	GeometryBuffer(GeometryBuffer&&);
	GeometryBuffer& operator=(GeometryBuffer&& other);

	BufferLocation AddRenderer(GameEngine::Renderer*);
	void Select();
};

#endif