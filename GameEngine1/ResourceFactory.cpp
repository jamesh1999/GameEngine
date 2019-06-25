#include "ResourceFactory.h"
#include "Resource.h"
#include "MeshLoader.h"
#include "TextureLoader.h"
#include "TextureArrayLoader.h"

using namespace GameEngine::Resources;
using namespace GameEngine;

ResourceFactory::ResourceFactory(Engine* e) : engine(e) {}

Resource* ResourceFactory::Deserialize(std::istream& in)
{
	std::string type;
	in >> type;

	Resource* ret = nullptr;
	if (type == typeid(Mesh).name()) ret = new Mesh;
	else if (type == typeid(Texture).name()) ret = new Texture;
	else if (type == typeid(TextureArray).name()) ret = new TextureArray;

	// Return to original position now type is known
	in.seekg(-static_cast<int>(type.size()), std::ios::cur);

	*ret << in;

	return ret;
}

template <>
Resource* ResourceFactory::Load<Resource>(const std::string& filename)
{
	std::ifstream in(filename);
	return Deserialize(in);
}

template <>
Texture* ResourceFactory::Load<Texture>(const std::string& descriptor)
{
	return TextureLoader::Load(descriptor);
}

template <>
TextureArray* ResourceFactory::Load<TextureArray>(const std::string& descriptor)
{
	return TextureArrayLoader::Load(engine, descriptor);
}

template <>
Mesh* ResourceFactory::Load<Mesh>(const std::string& descriptor)
{
	return MeshLoader::Load(descriptor);
}
