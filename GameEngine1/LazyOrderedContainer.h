#ifndef __LAZY_ORDERED_CONTAINER_INCLUDED__
#define __LAZY_ORDERED_CONTAINER_INCLUDED__

#include <algorithm>

namespace GameEngine
{
	namespace Utils
	{
		template <class T, class TCont, class TComp>
		class LazyOrderedContainer
		{
		private:
			mutable TCont m_container;
			TComp m_comp;

			bool m_dirty = false;

			void Sort() const
			{
				std::sort(m_container.begin(), m_container.end(), m_comp);
			}

		public:

			LazyOrderedContainer() = default;

			LazyOrderedContainer(TComp& comp)
			{
				m_comp = comp;
			}

			//Indexing is not allowed to change the elements in case that changes ordering
			const T& operator[](unsigned idx) const
			{
				if (m_dirty) Sort();
				return m_container[idx];
			}

			//Insert at the end and mark the container as dirty
			void Insert(const T& itm)
			{
				m_dirty = true;
				m_container.insert(m_container.end(), itm);
			}

			void Erase(const T& itm)
			{
				typename TCont::iterator it = std::find(m_container.begin(), m_container.end(), itm);
				m_container.erase(it);
			}

			void SetDirty()
			{
				m_dirty = true;
			}

			//Accessors to underlying container
			typename TCont::iterator begin()
			{
				if (m_dirty) Sort();
				return m_container.begin();
			}

			typename TCont::iterator end()
			{
				if (m_dirty) Sort();
				return m_container.end();
			}

			size_t Size() const
			{
				return m_container.size();
			}
		};
	}
}

#endif
