#include "MeshLoader.h"
#include <fstream>
#include <sstream>
#include <vector>
#include "DescriptorParser.h"

using namespace GameEngine::Resources;

//FBX loader

void MeshLoader::FBXGetVertexPos(FbxMesh* in, int ctrlPointIdx, DirectX::XMFLOAT3& out)
{
	out.x = static_cast<float>(in->GetControlPointAt(ctrlPointIdx).mData[0]) / 100.0f;
	out.y = static_cast<float>(in->GetControlPointAt(ctrlPointIdx).mData[1]) / 100.0f;
	out.z = static_cast<float>(in->GetControlPointAt(ctrlPointIdx).mData[2]) / 100.0f;
}

void MeshLoader::FBXGetNormal(FbxMesh* in, int ctrlPointIdx, int vtxIdx, DirectX::XMFLOAT3& out)
{
	FbxGeometryElementNormal* norm = in->GetElementNormal(0);
	switch (norm->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (norm->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
			{
				int idx = norm->GetIndexArray().GetAt(ctrlPointIdx);
				out.x = static_cast<float>(norm->GetDirectArray().GetAt(idx).mData[0]);
				out.y = static_cast<float>(norm->GetDirectArray().GetAt(idx).mData[1]);
				out.z = static_cast<float>(norm->GetDirectArray().GetAt(idx).mData[2]);
				break;
			}
		case FbxGeometryElement::eDirect:
			out.x = static_cast<float>(norm->GetDirectArray().GetAt(ctrlPointIdx).mData[0]);
			out.y = static_cast<float>(norm->GetDirectArray().GetAt(ctrlPointIdx).mData[1]);
			out.z = static_cast<float>(norm->GetDirectArray().GetAt(ctrlPointIdx).mData[2]);
			break;
		}
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (norm->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
			{
				int idx = norm->GetIndexArray().GetAt(vtxIdx);
				out.x = static_cast<float>(norm->GetDirectArray().GetAt(idx).mData[0]);
				out.y = static_cast<float>(norm->GetDirectArray().GetAt(idx).mData[1]);
				out.z = static_cast<float>(norm->GetDirectArray().GetAt(idx).mData[2]);
				break;
			}
		case FbxGeometryElement::eDirect:
			out.x = static_cast<float>(norm->GetDirectArray().GetAt(vtxIdx).mData[0]);
			out.y = static_cast<float>(norm->GetDirectArray().GetAt(vtxIdx).mData[1]);
			out.z = static_cast<float>(norm->GetDirectArray().GetAt(vtxIdx).mData[2]);
			break;
		}
		break;
	}
}

void MeshLoader::FBXGetUV(FbxMesh* in, int ctrlPointIdx, int vtxIdx, DirectX::XMFLOAT3& out)
{
	FbxGeometryElementUV* uv = in->GetElementUV(0);
	if (uv == nullptr)
		return;

	switch (uv->GetMappingMode())
	{
	case FbxGeometryElement::eByControlPoint:
		switch (uv->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
			{
				int idx = uv->GetIndexArray().GetAt(ctrlPointIdx);
				out.x = static_cast<float>(uv->GetDirectArray().GetAt(idx).mData[0]);
				out.y = 1.0f - static_cast<float>(uv->GetDirectArray().GetAt(idx).mData[1]);
				out.z = 0.0f;
				break;
			}
		case FbxGeometryElement::eDirect:
			out.x = static_cast<float>(uv->GetDirectArray().GetAt(ctrlPointIdx).mData[0]);
			out.y = 1.0f - static_cast<float>(uv->GetDirectArray().GetAt(ctrlPointIdx).mData[1]);
			out.z = 0.0f;
			break;
		}
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (uv->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
			{
				int idx = uv->GetIndexArray().GetAt(vtxIdx);
				out.x = static_cast<float>(uv->GetDirectArray().GetAt(idx).mData[0]);
				out.y = 1.0f - static_cast<float>(uv->GetDirectArray().GetAt(idx).mData[1]);
				out.z = 0.0f;
				break;
			}
		case FbxGeometryElement::eDirect:
			out.x = static_cast<float>(uv->GetDirectArray().GetAt(vtxIdx).mData[0]);
			out.y = 1.0f - static_cast<float>(uv->GetDirectArray().GetAt(vtxIdx).mData[1]);
			out.z = 0.0f;
			break;
		}
		break;
	}
}

void MeshLoader::ApplyFbxRecursive(Mesh* out, FbxNode* node, bool track)
{
	if (node->FindProperty("WOforwardTrackDirection").IsValid()
		&& !node->FindProperty("WOforwardTrackDirection").Get<FbxBool>())
		return;

	if (track && strncmp(node->GetName(), "collision_reverse", 17) == 0)
		return;

	FbxNodeAttribute* attr = node->GetNodeAttribute();
	if (attr)
	{
		if (attr->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			const char* name = node->GetName();
			if (track && strncmp(name, "collision_floor", 15) != 0 && strncmp(name, "collision_magfl", 15) != 0)
				return;

			FbxMesh* in = node->GetMesh();
			int lCnt = in->GetLayerCount();
			for (int j = 0; j < lCnt; ++j)
			{
				int polygonCount = in->GetPolygonCount();

				Vertex buff;
				int vtxIdx = 0;
				int vtxTot = out->vertices.size();
				for (int i = 0; i < polygonCount; ++i)
				{
					int vcnt = in->GetPolygonSize(i);
					for (int j = 0; j < vcnt; ++j)
					{
						int ctrlPointIdx = in->GetPolygonVertex(i, j);
						FBXGetVertexPos(in, ctrlPointIdx, buff.pos);
						FBXGetNormal(in, ctrlPointIdx, vtxIdx, buff.normal);
						FBXGetUV(in, ctrlPointIdx, vtxIdx, buff.tex);
						out->vertices.push_back(buff);

						++vtxIdx;
					}

					for (int j = 1; j < vcnt - 1; ++j)
					{
						out->indices.push_back(vtxTot);
						out->indices.push_back(vtxTot + j);
						out->indices.push_back(vtxTot + j + 1);
					}

					vtxTot += vcnt;
				}
			}
		}
	}
	int childCnt = node->GetChildCount();
	for (int i = 0; i < childCnt; ++i)
		ApplyFbxRecursive(out, node->GetChild(i), track);
}

void MeshLoader::LoadFBX(Mesh* out, FbxMesh* in)
{
	int lCnt = in->GetLayerCount();
	for (int j = 0; j < lCnt; ++j)
	{
		int polygonCount = in->GetPolygonCount();
		out->indices.reserve(polygonCount * 3);
		out->vertices.reserve(polygonCount * 3);

		Vertex buff;
		int vtxIdx = 0;
		int vtxTot = out->vertices.size();
		for (int i = 0; i < polygonCount; ++i)
		{
			int vcnt = in->GetPolygonSize(i);
			for (int j = 0; j < vcnt; ++j)
			{
				int ctrlPointIdx = in->GetPolygonVertex(i, j);
				FBXGetVertexPos(in, ctrlPointIdx, buff.pos);
				FBXGetNormal(in, ctrlPointIdx, vtxIdx, buff.normal);
				FBXGetUV(in, ctrlPointIdx, vtxIdx, buff.tex);
				out->vertices.push_back(buff);

				++vtxIdx;
			}

			for (int j = 1; j < vcnt - 1; ++j)
			{
				out->indices.push_back(vtxTot);
				out->indices.push_back(vtxTot + j);
				out->indices.push_back(vtxTot + j + 1);
			}

			vtxTot += vcnt;
		}
	}
}

void MeshLoader::LoadFBX(Mesh* mesh, FbxNode* node, const std::string& path)
{
	// Load from this node
	if (path == "")
	{
		FbxNodeAttribute* attr = node->GetNodeAttribute();
		if (attr == nullptr || attr->GetAttributeType() != FbxNodeAttribute::eMesh) return;

		LoadFBX(mesh, node->GetMesh());
		return;
	}

	size_t len = path.find_first_of("\\/");
	std::string childName = path.substr(0, len);
	std::string remainder = path.substr(len + 1, path.size() - 1 - len);

	// Traverse down heirarchy
	FbxNode* next = node->FindChild(childName.c_str(), false);

	if (next == nullptr) return;
	LoadFBX(mesh, next, remainder);
}

void MeshLoader::LoadFBX(Mesh* mesh, const std::string& filename, const std::string& path)
{
	// Create necessary resources
	FbxManager* fbx = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(fbx, IOSROOT);
	fbx->SetIOSettings(ios);
	FbxImporter* fbxImporter = FbxImporter::Create(fbx, "");

	bool success = fbxImporter->Initialize(filename.c_str(), -1, fbx->GetIOSettings());

	if (!success) return;

	FbxScene* fbxScene = FbxScene::Create(fbx, filename.c_str());
	fbxImporter->Import(fbxScene);
	fbxImporter->Destroy();

	LoadFBX(mesh, fbxScene->GetRootNode(), path);

	// Clean up
	fbx->Destroy();
}

void MeshLoader::LoadOBJ(Mesh* mesh, const std::string& filename)
{
	std::vector<DirectX::XMFLOAT3> vertexPos;
	std::vector<DirectX::XMFLOAT3> vertexTex;
	std::vector<DirectX::XMFLOAT3> vertexNorm;
	std::vector<int[3]> faces;

	std::ifstream ifile(filename);
	std::string line;
	std::stringstream ss;

	while (std::getline(ifile, line))
	{
		ss << line;

		std::string type;
		float data[3];
		ss >> type;
		
		if (type[0] == 'v')
			ss >> data[0] >> data[1] >> data[2];
		if (type == "v") vertexPos.push_back({ data[0], data[1], data[2] });
		else if (type == "vt") vertexTex.push_back({ data[0], data[1], data[2] });
		else if (type == "vn") vertexNorm.push_back({ data[0], data[1], data[2] });

		if (type == "f")
		{
			for (int i = 0; i < 3; ++i)
			{
				Vertex v;
				int x;
				char buff;
				ss >> x;
				v.pos = vertexPos[x];
				ss >> buff;
				ss >> x;
				v.tex = vertexTex[x];
				ss >> buff;
				ss >> x;
				v.normal = vertexNorm[x];

				mesh->vertices.push_back(v);
				mesh->indices.push_back(mesh->indices.size() - 1);
			}
		}

		ss.clear();
	}
}

void MeshLoader::Load(Mesh* mesh, const std::string& descriptor)
{
	std::vector<std::string> parts = DescriptorParser::GetParts(descriptor);

	std::string extension = DescriptorParser::GetExtension(parts[0]);
	if (extension == "fbx")
	{
		if (parts.size() == 1)
		{
			LoadFBX(mesh, parts[0]);
			return;
		}

		LoadFBX(mesh, parts[0], parts[1]);
		return;
	}

	if (extension == "obj")
	{
		LoadOBJ(mesh, parts[0]);
		return;
	}
}

void MeshLoader::Save(Mesh* mesh, const std::string& filename)
{

}