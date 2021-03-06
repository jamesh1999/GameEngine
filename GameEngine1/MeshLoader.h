#ifndef __MESH_LOADER_INCLUDED__
#define __MESH_LOADER_INCLUDED__

#include <fbxsdk.h>
#include <string>
#include <vector>
#include "Mesh.h"

namespace GameEngine
{
	namespace Resources
	{
		class MeshLoader
		{
		private:

			//FBX loader helper functions
			static void FBXGetVertexPos(FbxMesh*, int, DirectX::XMFLOAT3&);
			static void FBXGetNormal(FbxMesh*, int, int, DirectX::XMFLOAT3&);
			static void FBXGetUV(FbxMesh*, int, int, DirectX::XMFLOAT3&);

		public:
			// To remove
			static void ApplyFbxRecursive(Mesh*, FbxNode*, bool);
			// ---

		public:

			// FBX loader functions
			static void LoadFBX(Mesh*, FbxMesh*);
			static void LoadFBX(Mesh*, FbxNode*, const std::string& = "");
			static void LoadFBX(Mesh*, const std::string&, const std::string& = "");

			// OBJ loader functions
			static void LoadOBJ(Mesh*, const std::string&);

			static void Load(Mesh*, const std::string&);
			static void Save(Mesh*, const std::string&);
		};
	}
}

#endif
