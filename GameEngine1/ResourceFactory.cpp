#include "ResourceFactory.h"
#include "TextureLoader.h"

using namespace GameEngine::Resources;

template <>
Texture* ResourceFactory::Load<Texture>(const std::string& filename)
{
	return TextureLoader::LoadTexture(filename);
}