#ifndef __RESOURCE_PTR_INCLUDED__
#define __RESOURCE_PTR_INCLUDED__

#include "ElementPtr.h"
#include "Resource.h"

namespace GameEngine
{
	namespace Resources
	{
		template <class TResource>
		class ResourcePtr : public Elements::ElementPtr<TResource>
		{
		private:
			// Decrement the resource count and clean up
			void DecResource()
			{
				if (Get() == nullptr) return;

				static_cast<Resource*>(Get())->m_refCount -= 1;
				if (static_cast<Resource*>(Get())->m_refCount == 0)
					static_cast<Resource*>(Get())->DestroyResource();
			}

			// Increment the resource count
			void IncResource()
			{
				if (Get() == nullptr) return;

				static_cast<Resource*>(Get())->m_refCount += 1;
			}

		public:

			ResourcePtr(Engine* engine, Elements::Element::UID uid) : Elements::ElementPtr<TResource>(engine, uid)
			{
				static_assert(std::is_base_of<Resource, TResource>::value);
				IncResource();
			}
			ResourcePtr(TResource* resource) : Elements::ElementPtr<TResource>(resource)
			{
				static_assert(std::is_base_of<Resource, TResource>::value);
				IncResource();
			}
			ResourcePtr() : ElementPtr(nullptr) {}

			ResourcePtr(const ResourcePtr<TResource>& other) : Elements::ElementPtr<TResource>(other)
			{
				IncResource();
			}

			~ResourcePtr() { DecResource(); }

			ResourcePtr<TResource>& operator=(const ResourcePtr<TResource>& other)
			{
				DecResource();
				Elements::ElementPtr<TResource>::operator=(other);
				IncResource();

				return *this;
			}

			ResourcePtr<TResource>& operator=(TResource* resource)
			{
				DecResource();
				Elements::ElementPtr<TResource>::operator=(resource);
				IncResource();

				return *this;
			}
		};
	}
}

#endif
