#ifndef __RESOURCE_PTR_INCLUDED__
#define __RESOURCE_PTR_INCLUDED__

#include "Resource.h"

namespace GameEngine
{
	namespace Resources
	{
		template <class TResource>
		class ResourcePtr
		{
		private:
			TResource* m_resource = nullptr;

			// Decrement the resource count and clean up
			void DecResource()
			{
				if (m_resource == nullptr) return;

				static_cast<Resource*>(m_resource)->m_refCount -= 1;
				if (static_cast<Resource*>(m_resource)->m_refCount == 0)
					static_cast<Resource*>(m_resource)->Destroy();
			}

			// Increment the resource count
			void IncResource()
			{
				if (m_resource == nullptr) return;

				static_cast<Resource*>(m_resource)->m_refCount += 1;
			}

		public:

			ResourcePtr(TResource* resource)
			{
				static_assert(std::is_base_of<Resource, TResource>::value);

				m_resource = resource;
				IncResource();
			}
			ResourcePtr() : ResourcePtr(nullptr) {};

			~ResourcePtr() { DecResource(); }

			ResourcePtr(const ResourcePtr<TResource>& other)
			{
				m_resource = other.m_resource;
				IncResource();
			}
			ResourcePtr<TResource>& operator=(const ResourcePtr<TResource>& other)
			{
				DecResource();

				m_resource = other.m_resource;
				IncResource();

				return *this;
			}

			ResourcePtr<TResource>& operator=(TResource* resource)
			{
				DecResource();

				m_resource = resource;
				IncResource();

				return *this;
			}

			TResource* Get() const
			{
				return m_resource;
			}

			TResource* operator->() const
			{
				return Get();
			}

			TResource& operator*() const
			{
				return *Get();
			}
		};
	}
}

#endif