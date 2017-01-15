#include "MeshLoader.h"
#include <iostream>

MeshLoader* MeshLoader::instance = nullptr;

//FBX loader

void MeshLoader::FBXGetVertexPos(FbxMesh* in, int ctrlPointIdx, DirectX::XMFLOAT3& out)
{
	out.x = in->GetControlPointAt(ctrlPointIdx).mData[0] / 100;
	out.y = in->GetControlPointAt(ctrlPointIdx).mData[1] / 100;
	out.z = in->GetControlPointAt(ctrlPointIdx).mData[2] / 100;
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
			out.x = norm->GetDirectArray().GetAt(idx).mData[0];
			out.y = norm->GetDirectArray().GetAt(idx).mData[1];
			out.z = norm->GetDirectArray().GetAt(idx).mData[2];
			break;
		}
		case FbxGeometryElement::eDirect:
			out.x = norm->GetDirectArray().GetAt(ctrlPointIdx).mData[0];
			out.y = norm->GetDirectArray().GetAt(ctrlPointIdx).mData[1];
			out.z = norm->GetDirectArray().GetAt(ctrlPointIdx).mData[2];
			break;
		}
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (norm->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
		{
			int idx = norm->GetIndexArray().GetAt(vtxIdx);
			out.x = norm->GetDirectArray().GetAt(idx).mData[0];
			out.y = norm->GetDirectArray().GetAt(idx).mData[1];
			out.z = norm->GetDirectArray().GetAt(idx).mData[2];
			break;
		}
		case FbxGeometryElement::eDirect:
			out.x = norm->GetDirectArray().GetAt(vtxIdx).mData[0];
			out.y = norm->GetDirectArray().GetAt(vtxIdx).mData[1];
			out.z = norm->GetDirectArray().GetAt(vtxIdx).mData[2];
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
			out.x = uv->GetDirectArray().GetAt(idx).mData[0];
			out.y = uv->GetDirectArray().GetAt(idx).mData[1];
			break;
		}
		case FbxGeometryElement::eDirect:
			out.x = uv->GetDirectArray().GetAt(ctrlPointIdx).mData[0];
			out.y = uv->GetDirectArray().GetAt(ctrlPointIdx).mData[1];
			break;
		}
		break;
	case FbxGeometryElement::eByPolygonVertex:
		switch (uv->GetReferenceMode())
		{
		case FbxGeometryElement::eIndexToDirect:
		{
			int idx = uv->GetIndexArray().GetAt(vtxIdx);
			out.x = uv->GetDirectArray().GetAt(idx).mData[0];
			out.y = uv->GetDirectArray().GetAt(idx).mData[1];
			break;
		}
		case FbxGeometryElement::eDirect:
			out.x = uv->GetDirectArray().GetAt(vtxIdx).mData[0];
			out.y = uv->GetDirectArray().GetAt(vtxIdx).mData[1];
			break;
		}
		break;
	}
}

void MeshLoader::PrintFBXRecursive(FbxNode* node, int indentation)
{
	for (int i = 0; i < indentation; ++i)
		std::cout << "  ";

	std::cout << node->GetName() << std::endl;

	for (int i = 0; i < indentation; ++i)
		std::cout << "  ";

	FbxNodeAttribute* attr = node->GetNodeAttribute();
	std::cout << "Type: ";
	if (attr)
	{
		switch (attr->GetAttributeType())
		{
		case FbxNodeAttribute::eMesh:
			std::cout << "Mesh";
			break;
		case FbxNodeAttribute::eLight:
			std::cout << "Light";
			break;
		case FbxNodeAttribute::eCamera:
			std::cout << "Camera";
		}
	}
	else
		std::cout << "??";
	std::cout << std::endl;

	int childCnt = node->GetChildCount();
	for (int i = 0; i < childCnt; ++i)
		PrintFBXRecursive(node->GetChild(i), indentation + 1);
}

void MeshLoader::ApplyFbxRecursive(MeshData* out, FbxNode* node)
{
	FbxNodeAttribute* attr = node->GetNodeAttribute();
	if (attr)
	{
		if (attr->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			out->push_back(SubMesh());
			ApplyMesh(&(out->back()), node->GetMesh());
		}
	}
	int childCnt = node->GetChildCount();
	for (int i = 0; i < childCnt; ++i)
		ApplyFbxRecursive(out, node->GetChild(i));
}

void MeshLoader::ApplyFbxRecursiveWithTextures(MeshData* out, FbxNode* node, std::vector<std::string>& textures)
{
	std::vector<Material*> mats;
	FbxNodeAttribute* attr = node->GetNodeAttribute();
	if (attr)
	{
		if (attr->GetAttributeType() == FbxNodeAttribute::eMesh)
		{
			int lCnt = node->GetGeometry()->GetLayerCount();
			for (int j = 0; j < lCnt; ++j)
			{
				FbxLayerElementMaterial* lE = node->GetGeometry()->GetLayer(j)->GetMaterials();
				if (lE->GetMappingMode() == FbxLayerElement::eAllSame)
				{
					FbxSurfaceMaterial* surf = node->GetMaterial(lE->GetIndexArray()[0]);
					if (surf != nullptr)
					{
						FbxProperty prop = surf->FindProperty(FbxSurfaceMaterial::sDiffuse);
						FbxFileTexture* fTex = prop.GetSrcObject<FbxFileTexture>();
						if (fTex != nullptr)
						{
							out->push_back(SubMesh());
							ApplyMesh(&(out->back()), node->GetMesh());

							std::string name = fTex->GetFileName();

							auto it = std::find(textures.begin(), textures.end(), name);

							int idx = it - textures.begin();

							if (it == textures.end())
								textures.push_back(name);

							for (Vertex& v : out->back().vertices)
								v.tex.z = idx;
						}
					}
				}
				else if (lE->GetMappingMode() == FbxLayerElement::eByPolygon)
				{
					int matCnt = lE->GetIndexArray().GetCount();
					for (int i = 0; i < matCnt; ++i)
					{
						FbxSurfaceMaterial* surf = node->GetMaterial(lE->GetIndexArray()[i]);
						if (surf != nullptr)
						{
							FbxProperty prop = surf->FindProperty(FbxSurfaceMaterial::sDiffuse);
							FbxFileTexture* fTex = prop.GetSrcObject<FbxFileTexture>();
							if (fTex != nullptr)
							{
								out->push_back(SubMesh());
								ApplyMesh(&(out->back()), node->GetMesh(), i);

								std::string name = fTex->GetFileName();

								auto it = std::find(textures.begin(), textures.end(), name);

								int idx = it - textures.begin();

								if (it == textures.end())
									textures.push_back(name);

								for (Vertex& v : out->back().vertices)
									v.tex.z = idx;
							}
						}
					}
				}
			}
		}
	}
	int childCnt = node->GetChildCount();
	for (int i = 0; i < childCnt; ++i)
		ApplyFbxRecursiveWithTextures(out, node->GetChild(i), textures);
}

void MeshLoader::ApplyMesh(SubMesh* out, FbxMesh* mesh)
{
	unsigned polygonCount = mesh->GetPolygonCount();
	out->indices.reserve(polygonCount * 3);
	out->vertices.reserve(polygonCount * 3);

	Vertex buff;
	int vtxIdx = 0;
	int vtxTot = out->vertices.size();
	for (int i = 0; i < polygonCount; ++i)
	{
		int vcnt = mesh->GetPolygonSize(i);
		for (int j = 0; j < vcnt; ++j)
		{
			int ctrlPointIdx = mesh->GetPolygonVertex(i, j);
			FBXGetVertexPos(mesh, ctrlPointIdx, buff.pos);
			FBXGetNormal(mesh, ctrlPointIdx, vtxIdx, buff.normal);
			FBXGetUV(mesh, ctrlPointIdx, vtxIdx, buff.tex);
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

void MeshLoader::ApplyMesh(SubMesh* out, FbxMesh* mesh, int polygon)
{
	out->indices.reserve(3);
	out->vertices.reserve(3);

	Vertex buff;
	int vtxIdx = 0;
	int vtxTot = out->vertices.size();
	for (int i = 0; i < polygon; ++i)
	{
		vtxIdx += mesh->GetPolygonSize(i);
	}

	int vcnt = mesh->GetPolygonSize(polygon);
	for (int j = 0; j < vcnt; ++j)
	{
		int ctrlPointIdx = mesh->GetPolygonVertex(polygon, j);
		FBXGetVertexPos(mesh, ctrlPointIdx, buff.pos);
		FBXGetNormal(mesh, ctrlPointIdx, vtxIdx, buff.normal);
		FBXGetUV(mesh, ctrlPointIdx, vtxIdx, buff.tex);
		buff.tex.x *= -1;
		buff.tex.y *= -1;
		out->vertices.push_back(buff);

		++vtxIdx;
	}

	for (int j = 1; j < vcnt - 1; ++j)
	{
		out->indices.push_back(vtxTot);
		out->indices.push_back(vtxTot + j);
		out->indices.push_back(vtxTot + j + 1);
	}
}

MeshLoader::MeshLoader()
{
	if (instance != nullptr) return;

	instance = this;
}

FbxNode* MeshLoader::LoadFBX(std::string filename)
{
	FbxManager* fbx = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(fbx, IOSROOT);
	fbx->SetIOSettings(ios);
	FbxImporter* fbxImporter = FbxImporter::Create(fbx, "");

	bool success = fbxImporter->Initialize(filename.c_str(), -1, fbx->GetIOSettings());

	if (!success) return nullptr;

	FbxScene* fbxScene = FbxScene::Create(fbx, filename.c_str());
	fbxImporter->Import(fbxScene);
	fbxImporter->Destroy();

	return fbxScene->GetRootNode();
}

void MeshLoader::PrintFBXHeirachy(FbxNode* rootNode)
{
	PrintFBXRecursive(rootNode, 0);
}

void MeshLoader::ApplyFBX(MeshData* out, FbxNode* rootNode, std::string meshName)
{
	if (meshName == "")
		ApplyFbxRecursive(out, rootNode);
	else
		ApplyFbxRecursive(out, rootNode->FindChild(meshName.c_str()));
}

void MeshLoader::ApplyFBXWithTextures(MeshData* out, FbxNode* rootNode, std::string meshName, std::vector<std::string>& textures)
{
	if (meshName == "")
		ApplyFbxRecursiveWithTextures(out, rootNode, textures);
	else
		ApplyFbxRecursiveWithTextures(out, rootNode->FindChild(meshName.c_str()), textures);

	for (int i = 1; i < out->size(); ++i)
	{
		int idxOffset = (*out)[0].vertices.size();

		for (int j = 0; j < (*out)[i].vertices.size(); ++j)
			(*out)[0].vertices.push_back((*out)[i].vertices[j]);
		for (int j = 0; j < (*out)[i].indices.size(); ++j)
			(*out)[0].indices.push_back((*out)[i].indices[j] + idxOffset);
	}

	out->resize(1);
}

void MeshLoader::LoadOBJ(MeshData* mesh, std::string filename)
{
	
}
