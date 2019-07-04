#include "ResourceTable.h"
#include <sstream>
#include "Resource.h"
#include "ResourceFactory.h"

using namespace GameEngine;
using namespace GameEngine::Resources;

ResourceTable::ResourceTable(Engine* e) : engine(e) {}

ResourceTable& ResourceTable::operator<<(std::istream& in)
{
	int resourceCount;
	in >> resourceCount;
	
	for (int i = 0; i < resourceCount; ++i)
	{
		std::string res;
		in >> res;
		// TODO
		//engine->resourceFactory->Create<Resource>(res);
	}

	return *this;
}

ResourceTable& ResourceTable::operator>>(std::ostream& out)
{
	out << resources.size();
	for (auto kvp : resources)
		out << kvp.first;

	return *this;
}

Resource* ResourceTable::operator[](const std::string& identifier)
{
	return resources[identifier];
}

void ResourceTable::Erase(const std::string& identifier)
{
	resources.erase(identifier);
}

void ResourceTable::Erase(Resource* res)
{
	resources.erase(res->GetIdentifier());
}