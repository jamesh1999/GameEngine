#include "TextureArrayLoader.h"
#include <vector>
#include "DescriptorParser.h"
#include "ResourceFactory.h"
#include "Texture.h"

using namespace GameEngine;
using namespace Resources;

void TextureArrayLoader::Load(TextureArray* texArray, const std::string& descriptor)
{
	if (descriptor == "") return;

	std::vector<std::string> parts = DescriptorParser::GetParts(descriptor);
	std::vector<std::string> files = DescriptorParser::GetParts(parts[0]);

	for (std::string file : files)
	{
		Texture* tex = texArray->GetEngine()->resourceFactory->Create<Texture>(file);
		texArray->m_textures.push_back(tex);
		texArray->m_transparent |= !tex->IsOpaque();
	}
}
