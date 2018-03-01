#ifndef __RESOURCE_TABLE_INCLUDED__
#define __RESOURCE_TABLE_INCLUDED__

#include <vector>

namespace GameEngine
{
	namespace Resources
	{
		class Resource;

		class ResourceTable
		{
		public:
			std::vector<Resource*> resources;
		};
	}
}

#endif