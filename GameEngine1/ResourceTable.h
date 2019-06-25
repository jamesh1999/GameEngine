#ifndef __RESOURCE_TABLE_INCLUDED__
#define __RESOURCE_TABLE_INCLUDED__

#include <string>
#include <unordered_map>
#include "ISerializable.h"
#include "Engine.h"

namespace GameEngine
{
	namespace Resources
	{
		class Resource;
		class ResourceFactory;

		class ResourceTable : public Utils::ISerializable
		{
			friend class ResourceFactory;

		private:
			Engine* engine;
			std::unordered_map<std::string, Resource*> resources;

		public:

			ResourceTable(Engine*);

			ResourceTable& operator<<(std::istream&);
			ResourceTable& operator>>(std::ostream&);

			Resource* operator[](const std::string&);
			void Erase(const std::string&);
			void Erase(Resource*);
		};
	}
}

#endif
