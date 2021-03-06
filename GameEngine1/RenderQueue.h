#ifndef __RENDER_QUEUE_INCLUDED__
#define __RENDER_QUEUE_INCLUDED__

#include <DirectXMath.h>
#include <unordered_map>
#include "LazyOrderedContainer.h"
#include "Renderer.h"

namespace GameEngine
{
	namespace Graphics
	{
		class RenderQueue
		{
		private:

			class RQComp
			{
			public:
				std::unordered_map<unsigned, int>* m_priorities;

				bool operator()(Renderer* a, Renderer* b)
				{
					return (*m_priorities)[a->GetUID()] > (*m_priorities)[b->GetUID()];
				}
			};

			std::unordered_map<Elements::Element::UID, int> m_priorities;
			Utils::LazyOrderedContainer<Renderer*, std::vector<Renderer*>, RQComp> m_queue;

		public:
			RenderQueue();

			void AddRenderer(Renderer*);
			void RemoveRenderer(Renderer*);

			void Refresh(DirectX::XMVECTOR);

			const std::vector<Renderer*>::iterator begin();
			const std::vector<Renderer*>::iterator end();
		};
	}
}

#endif
