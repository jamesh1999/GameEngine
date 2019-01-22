#ifndef __RESOURCE_REF_INCLUDED__
#define __RESOURCE_REF_INCLUDED__

#include "Resource.h"

namespace GameEngine
{
	namespace Resources
	{
		template <class TResource>
		class ResourceRef
		{

		private:
			TResource* m_resource = nullptr;

			// Decrement the resource count and clean up
			void ResourceRef<TResource>::DecResource()
			{
				if (m_resource == nullptr) return;

				static_cast<Resource*>(m_resource)->m_refCount -= 1;
				if (static_cast<Resource*>(m_resource)->m_refCount == 0)
					static_cast<Resource*>(m_resource)->Destroy();
			}

			// Increment the resource count
			void ResourceRef<TResource>::IncResource()
			{
				if (m_resource == nullptr) return;

				static_cast<Resource*>(m_resource)->m_refCount += 1;
			}

		public:

			ResourceRef()
			{
				static_assert(std::is_base_of<Resource, TResource>::value);
			}

			ResourceRef(TResource* resource)
			{
				static_assert(std::is_base_of<Resource, TResource>::value);

				m_resource = resource;
				IncResource();
			}

			ResourceRef(const ResourceRef<TResource>& other)
			{
				DecResource();

				m_resource = other.m_resource;
				IncResource();
			}

			~ResourceRef() { DecResource(); }

			ResourceRef<TResource>& operator=(const ResourceRef<TResource>& other)
			{
				DecResource();

				m_resource = other.m_resource;
				IncResource();

				return *this;
			}

			ResourceRef<TResource>& operator=(TResource* resource)
			{
				DecResource();

				m_resource = resource;
				IncResource();

				return *this;
			}

			TResource* operator->() const
			{
				return m_resource;
			}

			TResource* operator*() const
			{
				return m_resource;
			}
		};
	}
}

#endif