#ifndef __RESOURCE_INCLUDED__
#define __RESOURCE_INCLUDED__

#include <string>
#include "Engine.h"
#include "Element.h"

#define RegisterResource(resource, loader) \
	{ \
		RegisterElement(resource); \
		GameEngine::Resources::ResourceFactory::_RegisterResource( \
			typeid(resource).name(), \
			[](GameEngine::Resources::Resource* r, const std::string& d) { loader::Load(static_cast<resource*>(r), d); } \
		); \
	} 0

namespace GameEngine
{
	namespace Resources
	{
		class ResourceFactory;
		template <class TResource>
		class ResourcePtr;

		// A reference counted Element representing disk based data
		class Resource : public Elements::Element
		{
			friend class ResourceFactory;
			template <class>
			friend class ResourcePtr;

		private:
			unsigned m_refCount = 0;

			// Should only be accessible through ResourcePtr<> for clean up!!
			void DestroyResource();

		protected:
			std::string m_identifier;

			virtual Resource* CloneResource() = 0;

		public:

			// To hide Element::Destroy
			void Destroy() override;

			Resource& operator>>(std::ostream&);
			Resource& operator<<(std::istream&);

			std::string GetIdentifier() const;
			void Save(const std::string&);
		};
	}
}

#endif
