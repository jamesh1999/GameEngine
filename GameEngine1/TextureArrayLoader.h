#ifndef __TEXTURE_ARRAY_LOADER_INCLUDED__
#define __TEXTURE_ARRAY_LOADER_INCLUDED__

#include <string>
#include "TextureArray.h"
#include "Engine.h"

namespace GameEngine
{
	namespace Resources
	{
		class TextureArrayLoader
		{
		public:
			static TextureArray* Load(Engine*, const std::string&);
		};
	}
}

#endif
