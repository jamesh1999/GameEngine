#ifndef __ORDERED_CONTAINER_INCLUDED__
#define __ORDERED_CONTAINER_INCLUDED__

namespace GameEngine
{
	namespace Utils
	{
		template <class T, class TCont, class TComp>
		class OrderedContainer
		{
		private:
			TCont container;
			TComp comp;

		public:

			//Indexing is not allowed to change the elements in case that changes ordering
			const T& operator[](unsigned idx) const
			{
				return container[idx];
			}

			//Binary search for elements position then insert there
			void Insert(const T& itm)
			{
				unsigned upper = container.size();
				unsigned lower = 0;
				unsigned mid = 0;
				while (upper - lower > 0)
				{
					mid = (upper + lower) >> 1;

					if (comp(itm, container[mid]))
						lower = mid + 1;
					else
						upper = mid;
				}

				container.insert(container.begin() + lower, itm);
			}

			//Accessors to underlying container

			typename const TCont::iterator begin()
			{
				return container.begin();
			}

			typename const TCont::iterator end()
			{
				return container.end();
			}

			size_t Size() const
			{
				return container.size();
			}
		};
	}
}

#endif