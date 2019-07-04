#include "ResourceFactory.h"
#include "Resource.h"

using namespace GameEngine::Resources;

std::unordered_map<std::string, void(*)(Resource*, const std::string&)> ResourceFactory::resources;

void ResourceFactory::_RegisterResource(std::string name, void(*loader)(Resource*, const std::string&))
{
	resources[name] = loader;
}

Resource* ResourceFactory::DeserialiseHeader(std::istream& in, Resource* resource)
{
	std::stringstream ss;
	ss << resource->GetUID();
	std::string identifier = ss.str();

	static_cast<Resource*>(resource)->m_identifier = ss.str();
	GetEngine()->resources->resources[ss.str()] = static_cast<Resource*>(resource);
}

Resource* ResourceFactory::Deserialize(std::istream& in)
{
	// Read element/resource headers then continue as normal
	Resource* resource = static_cast<Resource*>(ElementFactory::DeserialiseHeader(in));
	DeserialiseHeader(in, resource);
	
	*resource << in;
	return resource;
}