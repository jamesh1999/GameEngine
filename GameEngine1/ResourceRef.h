#ifndef __RESOURCE_REF_INCLUDED__
#define __RESOURCE_REF_INCLUDED__

#include "Resource.h"

namespace GameEngine
{
	namespace Resources
	{
		template <typename TResource>
		class ResourceRef
		{

		private:
			TResource* m_resource = nullptr;

		public:

			ResourceRef() = default;

			~ResourceRef()
			{
				if (m_resource != nullptr) static_cast<Resource*>(m_resource)->m_refCount -= 1;
			}

			ResourceRef(const ResourceRef<TResource>& other)
			{
				if (m_resource != nullptr)
					static_cast<Resource*>(m_resource)->m_refCount -= 1;

				m_resource = other.m_resource;
				static_cast<Resource*>(m_resource)->m_refCount += 1;
			}

			ResourceRef& operator=(ResourceRef<TResource>& other)
			{
				if (m_resource != nullptr)
					static_cast<Resource*>(m_resource)->m_refCount -= 1;

				m_resource = other.m_resource;
				static_cast<Resource*>(m_resource)->m_refCount += 1;

				return other;
			}

			const ResourceRef& operator=(const ResourceRef<TResource>& other)
			{
				if (m_resource != nullptr)
					static_cast<Resource*>(m_resource)->m_refCount -= 1;

				m_resource = other.m_resource;
				static_cast<Resource*>(m_resource)->m_refCount += 1;

				return other;
			}

			ResourceRef(TResource* resource)
			{
				static_assert(std::is_base_of<Resource, TResource>::value);

				m_resource = resource;
				static_cast<Resource*>(m_resource)->m_refCount += 1;
			}

			TResource* operator=(TResource* resource)
			{
				if (m_resource != nullptr)
					static_cast<Resource*>(m_resource)->m_refCount -= 1;

				m_resource = resource;
				static_cast<Resource*>(m_resource)->m_refCount += 1;

				return m_resource;
			}

			TResource* operator->() const
			{
				return m_resource;
			}
		};
	}
}

#endif