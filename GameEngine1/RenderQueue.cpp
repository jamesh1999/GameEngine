#include "RenderQueue.h"
#include "CompositeObject.h"
#include "Transform.h"

using namespace GameEngine::Graphics;

RenderQueue::RenderQueue()
{
	RQComp cmp;
	cmp.m_priorities = &this->m_priorities;

	m_queue = Utils::LazyOrderedContainer<Renderer*, std::vector<Renderer*>, RQComp>(cmp);
}

void RenderQueue::AddRenderer(GameEngine::Renderer* r)
{
	m_priorities[r->GetID()] = 0.0f;
	m_queue.Insert(r);
}

void RenderQueue::RemoveRenderer(GameEngine::Renderer* r)
{
	m_priorities.erase(r->GetID());
	m_queue.Erase(r);
}

void RenderQueue::Refresh(DirectX::XMVECTOR forwards)
{
	for (Renderer* r : m_queue)
	{
		float result = DirectX::XMVectorGetX(
			DirectX::XMVector3Dot(
				DirectX::XMVector3Transform(
					r->mesh->GetCentroid(),
					r->obj->GetComponent<Transform>()->GetTransform()),
				forwards));
		unsigned priority = *reinterpret_cast<unsigned*>(&result);

		// Opaque near->far followed by transparent far->near
		priority >>= 1;
		if (priority & 0x40000000 == 0) priority ^= 0x3fffffff;
		if (r->GetTransparent()) priority = ~priority;

		m_priorities[r->GetID()] = *reinterpret_cast<int*>(&priority);
	}

	m_queue.SetDirty();
}

const std::vector<GameEngine::Renderer*>::iterator RenderQueue::begin()
{
	return m_queue.begin();
}

const std::vector<GameEngine::Renderer*>::iterator RenderQueue::end()
{
	return m_queue.end();
}