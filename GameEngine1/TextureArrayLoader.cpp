#include "TextureArrayLoader.h"
#include <vector>
#include "DescriptorParser.h"
#include "ResourceFactory.h"
#include "Texture.h"

using namespace GameEngine;
using namespace Resources;

TextureArray* TextureArrayLoader::Load(Engine* engine, const std::string& descriptor)
{
	TextureArray* texArray = new TextureArray;

	if (descriptor == "") return texArray;

	std::vector<std::string> parts = DescriptorParser::GetParts(descriptor);
	std::vector<std::string> files = DescriptorParser::GetParts(parts[0]);

	for (std::string file : files)
	{
		Texture* tex = engine->resourceFactory->Create<Texture>(file);
		texArray->m_textures.push_back(tex);
		texArray->m_transparent |= !tex->IsOpaque();
	}

	return texArray;
}
