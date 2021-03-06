#ifndef __TEXTURE_ARRAY_LOADER_INCLUDED__
#define __TEXTURE_ARRAY_LOADER_INCLUDED__

#include <string>
#include "Engine.h"
#include "TextureArray.h"

namespace GameEngine
{
	namespace Resources
	{
		class TextureArrayLoader
		{
		public:
			static void Load(TextureArray*, const std::string&);
		};
	}
}

#endif
