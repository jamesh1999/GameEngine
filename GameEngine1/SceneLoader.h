#ifndef __SCENE_LOADER_INCLUDED__
#define __SCENE_LOADER_INCLUDED__

#include <fbxsdk.h>
#include <string>
#include "CompositeObject.h"
#include "Engine.h"

namespace GameEngine
{
	namespace Resources
	{
		class SceneLoader
		{
		private:
			static Elements::CompositeObject* ApplyFBXRecursively(Engine*, FbxNode*);

		public:
			static Elements::CompositeObject* LoadFBX(Engine*, FbxScene*);
			static Elements::CompositeObject* LoadFBX(Engine*, const std::string&);
		};
	}
}

#endif
