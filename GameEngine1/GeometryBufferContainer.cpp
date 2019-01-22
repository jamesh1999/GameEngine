#include "GeometryBufferContainer.h"
#include "CompositeObject.h"

GeometryBufferContainer::GeometryBufferContainer(ID3D11Device* d, ID3D11DeviceContext* dc) : dev(d), devContext(dc)
{
	dev->AddRef();
	devContext->AddRef();

	// Create the static buffer
	m_buffers.emplace_back(dev, devContext, true);
}

GeometryBufferContainer::~GeometryBufferContainer()
{
	dev->Release();
	devContext->Release();
}

void GeometryBufferContainer::AddRenderer(GameEngine::Renderer* r)
{
	if (r->obj->GetComponent<Transform>()->m_static)
		m_lookup.insert(std::make_pair(r->GetID(), std::make_pair(0, m_buffers[0].AddRenderer(r))));
	else
	{
		m_buffers.emplace_back(dev, devContext);
		m_lookup.insert(std::make_pair(r->GetID(), std::make_pair(m_buffers.size() - 1, m_buffers.back().AddRenderer(r))));
	}
}

void GeometryBufferContainer::RemoveRenderer(GameEngine::Renderer* r)
{
	GeometryBuffer::BufferLocation pos;
	unsigned buff;

	auto it = m_lookup.find(r->GetID());
	if (it == m_lookup.end()) return;

	std::tie(buff, pos) = std::get<1>(*it);

	// TODO: Remove from buffer and free buffer if empty
}

GeometryBuffer::BufferLocation GeometryBufferContainer::FindRenderer(GameEngine::Renderer* r)
{
	GeometryBuffer::BufferLocation pos;
	unsigned buff;

	auto it = m_lookup.find(r->GetID());
	if (it == m_lookup.end()) return std::make_tuple(-1,-1);
	std::tie(buff, pos) = std::get<1>(*it);

	if (m_selected == -1 || m_selected != buff)
	{
		m_selected = buff;
		m_buffers[buff].Select();
	}

	return pos;
}

GeometryBuffer::BufferLocation GeometryBufferContainer::SelectGeometry(GameEngine::Renderer* r)
{
	GeometryBuffer::BufferLocation pos;
	unsigned buff;

	auto it = m_lookup.find(r->GetID());
	if (it == m_lookup.end()) return std::make_tuple(-1, -1);
	std::tie(buff, pos) = std::get<1>(*it);

	// Ensure buffer selected
	if (m_selected != buff)
	{
		m_buffers[buff].Select();
		m_selected = buff;
	}

	return pos;
}

void GeometryBufferContainer::Deselect()
{
	m_selected = -1;
}