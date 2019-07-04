#ifndef __RESOURCE_TABLE_INCLUDED__
#define __RESOURCE_TABLE_INCLUDED__

#include <string>
#include <unordered_map>
#include "Engine.h"

namespace GameEngine
{
	namespace Resources
	{
		class Resource;
		class ResourceFactory;

		class ResourceTable
		{
			friend class ResourceFactory;

		private:
			Engine* engine;
			std::unordered_map<std::string, Resource*> resources;

		public:

			ResourceTable(Engine*);

			Resource* operator[](const std::string&);
			void Erase(const std::string&);
			void Erase(Resource*);

			std::unordered_map<std::string, Resource*>::iterator begin();
			std::unordered_map<std::string, Resource*>::iterator end();
		};
	}
}

#endif
