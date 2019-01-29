#ifndef __MESH_LOADER_INCLUDED__
#define __MESH_LOADER_INCLUDED__

#include <string>
#include <fbxsdk.h>
#include "Mesh.h"
#include "Material.h"
#include <vector>

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
			static Mesh* LoadFBX(FbxMesh*);
			static Mesh* LoadFBX(FbxNode*, const std::string& = "");
			static Mesh* LoadFBX(const std::string&, const std::string& = "");

			// OBJ loader functions
			static Mesh* LoadOBJ(const std::string&);

			static Mesh* Load(const std::string&);
		};
	}
}

#endif
