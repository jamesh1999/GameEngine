#ifndef __GEOMETRY_BUFFER_CONTAINER_INCLUDED__
#define __GEOMETRY_BUFFER_CONTAINER_INCLUDED__

#include <unordered_map>
#include <d3d11.h>
#include "GeometryBuffer.h"
#include "Renderer.h"

class GeometryBufferContainer
{
private:
	std::vector<GeometryBuffer> m_buffers;
	std::unordered_map<unsigned, std::pair<unsigned, GeometryBuffer::BufferLocation>> m_lookup;

	ID3D11Device* dev;
	ID3D11DeviceContext* devContext;

public:

	GeometryBufferContainer(ID3D11Device*, ID3D11DeviceContext*);

	void AddRenderer(Renderer*);
	GeometryBuffer::BufferLocation FindRenderer(Renderer*);
};

#endif