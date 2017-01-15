#ifndef __MESH_LOADER_INCLUDED__
#define __MESH_LOADER_INCLUDED__

#include <string>
#include <fbxsdk.h>
#include "MeshData.h"
#include "Material.h"
#include <vector>

class MeshLoader
{
private:
	static MeshLoader* instance;

	//FBX loader helper functions
	static void FBXGetVertexPos(FbxMesh*, int, DirectX::XMFLOAT3&);
	static void FBXGetNormal(FbxMesh*, int, int, DirectX::XMFLOAT3&);
	static void FBXGetUV(FbxMesh*, int, int, DirectX::XMFLOAT3&);

	static void PrintFBXRecursive(FbxNode*, int);
	static void ApplyFbxRecursive(MeshData*, FbxNode*);

	static void ApplyFbxRecursiveWithTextures(MeshData*, FbxNode*, std::vector<std::string>&);

public:
	MeshLoader();

	static FbxNode* LoadFBX(std::string);
	static void PrintFBXHeirachy(FbxNode*);
	static void ApplyFBX(MeshData*, FbxNode*, std::string);
	static void ApplyFBXWithTextures(MeshData*, FbxNode*, std::string, std::vector<std::string>&);
	static void ApplyMesh(SubMesh*, FbxMesh*);
	static void ApplyMesh(SubMesh*, FbxMesh*, int);

	static void LoadOBJ(MeshData*,std::string);
};

#endif