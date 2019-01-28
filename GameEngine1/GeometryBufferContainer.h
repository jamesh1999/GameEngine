#ifndef __GEOMETRY_BUFFER_CONTAINER_INCLUDED__
#define __GEOMETRY_BUFFER_CONTAINER_INCLUDED__

#include <unordered_map>
#include <d3d11.h>
#include "GeometryBuffer.h"
#include "Renderer.h"
#include "Mesh.h"

class GeometryBufferContainer
{
private:
	std::vector<GeometryBuffer> m_buffers;
	std::unordered_map<unsigned, std::pair<unsigned, GeometryBuffer::BufferLocation>> m_lookup;
	unsigned m_selected = -1;

	ID3D11Device* dev;
	ID3D11DeviceContext* devContext;

public:

	GeometryBufferContainer(ID3D11Device*, ID3D11DeviceContext*);
	~GeometryBufferContainer();

	// To remove ---
	void AddRenderer(GameEngine::Renderer*);
	void RemoveRenderer(GameEngine::Renderer*);
	GeometryBuffer::BufferLocation FindRenderer(GameEngine::Renderer*);
	// ---

	/*void AddGeometry(GameEngine::Renderer*, Vertex*, int, int, int);
	void UpdateGeometry(GameEngine::Renderer*, Vertex*, int, int, int);
	void RemoveGeometry(GameEngine::Renderer*);*/

	GeometryBuffer::BufferLocation SelectGeometry(GameEngine::Renderer*);
	void Deselect();
};

#endif