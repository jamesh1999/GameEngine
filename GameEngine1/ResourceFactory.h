#ifndef __RESOURCE_FACTORY_INCLUDED__
#define __RESOURCE_FACTORY_INCLUDED__

#include <sstream>
#include <unordered_map>
#include "ElementFactory.h"
#include "Engine.h"
#include "ResourceTable.h"

namespace GameEngine
{
	namespace Resources
	{
		class Resource;

		// Handles initialisation of resources
		class ResourceFactory : public Elements::ElementFactory
		{
		private:
			template <class TResource>
			void Load(TResource*, const std::string&);

			static std::unordered_map<std::string, void (*)(Resource*, const std::string&)> resources;

		public:
			ResourceFactory(Engine* engine) : Elements::ElementFactory(engine) {};

			static void _RegisterResource(std::string, void(*)(Resource*, const std::string&));

			template <class TResource>
			TResource* Create(std::string = "");
			Resource* Deserialize(std::istream&);
		};

		//Set up resource
		template <class TResource>
		TResource* ResourceFactory::Create(std::string identifier)
		{
			static_assert(std::is_base_of<Resource, TResource>::value, "");

			// Resource is already loaded
			if (GetEngine()->resources->resources.find(identifier) != GetEngine()->resources->resources.end())
				return static_cast<TResource*>(GetEngine()->resources->resources[identifier]);

			TResource* resource = ElementFactory::Create<TResource>();
			Load<TResource>(resource, identifier);

			// Don't store failed resources
			if (resource == nullptr) return nullptr;

			// For unnamed resources use their UID
			if (identifier == "")
			{
				std::stringstream ss;
				ss << resource->GetUID();
				identifier = ss.str();
			}

			static_cast<Resource*>(resource)->m_identifier = identifier;
			GetEngine()->resources->resources[identifier] = static_cast<Resource*>(resource);

			return resource;
		}

		//Load the resource from disk (specialise per type)
		template <class TResource>
		void ResourceFactory::Load(TResource* resource, const std::string& descriptor)
		{
			resources[typeid(TResource).name()](resource, descriptor);
		}
	}
}

#endif
