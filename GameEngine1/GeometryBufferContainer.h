#ifndef __GEOMETRY_BUFFER_CONTAINER_INCLUDED__
#define __GEOMETRY_BUFFER_CONTAINER_INCLUDED__

#include <unordered_map>
#include "GeometryBuffer.h"
#include "Renderer.h"

class GeometryBufferContainer
{
private:
	std::vector<GeometryBuffer> m_buffers;
	std::unordered_map<unsigned, std::pair<unsigned, GeometryBuffer::BufferLocation>> m_lookup;

public:

	GeometryBufferContainer();

	void AddRenderer(Renderer*);
	GeometryBuffer::BufferLocation FindRenderer(Renderer*);
};

#endif