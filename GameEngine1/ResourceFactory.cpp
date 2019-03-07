#include "ResourceFactory.h"
#include "MeshLoader.h"
#include "TextureLoader.h"
#include "TextureArrayLoader.h"

using namespace GameEngine::Resources;
using namespace GameEngine;

ResourceFactory::ResourceFactory(Engine* e) : engine(e) {}

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
