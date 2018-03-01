#ifndef __RESOURCE_FACTORY_INCLUDED__
#define __RESOURCE_FACTORY_INCLUDED__

#include "Engine.h"
#include "ResourceTable.h"
#include "Texture.h"

namespace GameEngine
{
	namespace Resources
	{
		class ResourceFactory
		{
			friend class Engine;

		private:
			Engine* engine;

			template <class TResource>
			TResource* Load(const std::string&);

		public:

			template <class TResource>
			TResource* Create(const std::string&);
		};

		//Set up resource
		template <class TResource>
		TResource* ResourceFactory::Create(const std::string& filename)
		{
			static_assert(std::is_base_of<Resource, TResource>::value, "");

			TResource* resource = Load<TResource>(filename);
			static_cast<Resource*>(resource)->engine = engine;

			engine->resources->resources.push_back(static_cast<Resource*>(resource));

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
	}
}

#endif