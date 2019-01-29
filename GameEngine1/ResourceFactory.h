#ifndef __RESOURCE_FACTORY_INCLUDED__
#define __RESOURCE_FACTORY_INCLUDED__

#include <sstream>
#include <unordered_map>
#include "Engine.h"
#include "Mesh.h"
#include "ResourceTable.h"
#include "Texture.h"
#include "TextureArray.h"

namespace GameEngine
{
	namespace Resources
	{
		class ResourceFactory
		{
			friend class Engine;

		private:
			Engine* engine;
			uint64_t resourceCount = 0;

			template <class TResource>
			TResource* Load(const std::string&);

		public:

			template <class TResource>
			TResource* Create(std::string = "");
		};

		//Set up resource
		template <class TResource>
		TResource* ResourceFactory::Create(std::string identifier)
		{
			static_assert(std::is_base_of<Resource, TResource>::value, "");

			// Resource is already loaded
			if (engine->resources->resources.find(identifier) != engine->resources->resources.end())
				return static_cast<TResource*>(engine->resources->resources[identifier]);

			TResource* resource = Load<TResource>(identifier);

			// For unnamed resources generate a unique identifier
			if (identifier == "")
			{
				std::stringstream ss;
				ss << resourceCount;
				identifier = ss.str();

				++resourceCount;
			}

			// Don't store failed resources
			if (resource == nullptr) return nullptr;

			static_cast<Resource*>(resource)->m_identifier = identifier;
			static_cast<Resource*>(resource)->engine = engine;
			engine->resources->resources[identifier] = static_cast<Resource*>(resource);

			return resource;
		}

		//Load the resource from disk (specialise per type)
		template <class TResource>
		TResource* ResourceFactory::Load(const std::string&)
		{
			return new TResource;
		}

		template <>
		Texture* ResourceFactory::Load<Texture>(const std::string&);

		template <>
		TextureArray* ResourceFactory::Load<TextureArray>(const std::string&);

		template <>
		Mesh* ResourceFactory::Load<Mesh>(const std::string&);
	}
}

#endif
