#include "GeometryBufferContainer.h"
#include "CompositeObject.h"

GeometryBufferContainer::GeometryBufferContainer()
{
	m_buffers.emplace_back(true);
}

void GeometryBufferContainer::AddRenderer(Renderer* r)
{
	if (r->obj->GetComponent<Transform>()->m_static)
		m_lookup.insert(std::make_pair(r->GetID(), std::make_pair(0, m_buffers[0].AddRenderer(r))));
	else
	{
		m_buffers.emplace_back();
		m_lookup.insert(std::make_pair(r->GetID(), std::make_pair(m_buffers.size() - 1, m_buffers.back().AddRenderer(r))));
	}
}

GeometryBuffer::BufferLocation GeometryBufferContainer::FindRenderer(Renderer* r)
{
	GeometryBuffer::BufferLocation pos;
	unsigned buff;

	auto it = m_lookup.find(r->GetID());
	if (it == m_lookup.end()) return std::make_tuple(-1,-1);
	std::tie(buff, pos) = std::get<1>(*it);

	m_buffers[buff].Select();

	return pos;
}