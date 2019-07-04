#include "ResourceTable.h"
#include <sstream>
#include "Resource.h"
#include "ResourceFactory.h"

using namespace GameEngine;
using namespace GameEngine::Resources;

ResourceTable::ResourceTable(Engine* e) : engine(e) {}

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

std::unordered_map<std::string, Resource*>::iterator ResourceTable::begin()
{
	return resources.begin();
}

std::unordered_map<std::string, Resource*>::iterator ResourceTable::end()
{
	return resources.end();
}