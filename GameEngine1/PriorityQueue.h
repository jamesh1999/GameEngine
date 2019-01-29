#ifndef __PRIORITY_QUEUE_INCLUDED__
#define __PRIORITY_QUEUE_INCLUDED__

#include <utility>
#include <vector>
#include "OrderedContainer.h"

namespace GameEngine
{
	namespace Utils
	{
		namespace
		{
			template <class T>
			struct PQComp
			{
				bool operator()(const std::pair<int, T>& a, const std::pair<int, T>& b)
				{
					return a.first > b.first;
				}
			};
		}

		template <class T>
		using PriorityQueue = OrderedContainer<std::pair<int, T>, std::vector<std::pair<int, T>>, PQComp<T>>;
	}
}

#endif
