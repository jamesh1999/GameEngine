#ifndef __RESOURCE_TABLE_INCLUDED__
#define __RESOURCE_TABLE_INCLUDED__

#include <unordered_map>
#include <string>

namespace GameEngine
{
	namespace Resources
	{
		class Resource;

		class ResourceTable
		{
		public:
			std::unordered_map<std::string, Resource*> resources;
		};
	}
}

#endif
