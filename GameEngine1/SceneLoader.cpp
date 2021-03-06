#include "SceneLoader.h"
#include <DirectXMath.h>
#include "Component.h"
#include "ElementFactory.h"
#include "GraphicsController.h"
#include "Light.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshLoader.h"
#include "PropertyDict.h"
#include "Renderer.h"
#include "ResourceFactory.h"
#include "Transform.h"
#include "ResourcePtr.h"


using namespace GameEngine::Resources;
using namespace GameEngine;

Elements::CompositeObject* SceneLoader::ApplyFBXRecursively(Engine* engine, FbxNode* node)
{
	Elements::CompositeObject* co = engine->elementFactory->Create<Elements::CompositeObject>();

	// Set node transform values
	Elements::Transform* t = co->GetComponent<Elements::Transform>();
	DirectX::XMFLOAT3A v;
	DirectX::XMVECTOR vec;

	FbxVector4 pos = node->EvaluateLocalTranslation();
	v.x = static_cast<float>(pos.mData[0]);
	v.y = static_cast<float>(pos.mData[1]);
	v.z = static_cast<float>(pos.mData[2]);
	vec = XMLoadFloat3A(&v);
	t->SetPosition(vec);

	FbxVector4 rot = node->EvaluateLocalRotation();
	v.x = static_cast<float>(rot.mData[0]);
	v.y = static_cast<float>(rot.mData[1]);
	v.z = static_cast<float>(rot.mData[2]);
	vec = XMLoadFloat3A(&v);
	t->SetRotation(vec);

	FbxVector4 scale = node->EvaluateLocalScaling();
	v.x = 1; // scale.mData[0];
	v.y = 1; // scale.mData[1];
	v.z = 1; // scale.mData[2];
	vec = XMLoadFloat3A(&v);
	t->SetScale(vec);

	// Recurse through children and set their parents
	int childCnt = node->GetChildCount();
	for (int i = 0; i < childCnt; ++i)
	{
		Elements::CompositeObject* child = ApplyFBXRecursively(engine, node->GetChild(i));
		child->GetComponent<Elements::Transform>()->SetParent(t);
	}

	// Add properties to PropertyDict
	FbxProperty prop = node->FindProperty("WOforwardTrackDirection");
	if (prop.IsValid())
	{
		auto dict = co->AttachComponent<Elements::PropertyDict>();
		dict->SetProperty("WOforwardTrackDirection", prop.Get<FbxBool>() ? "true" : "false");
	}

	// Determine type of this node
	// TODO: Support multiple attributes?
	FbxNodeAttribute* attr = node->GetNodeAttribute();
	if (attr == nullptr) return co; // Nothing to do

	switch (attr->GetAttributeType())
	{
	case FbxNodeAttribute::eMesh:
		{
			int lCnt = node->GetGeometry()->GetLayerCount();
			for (int i = 0; i < lCnt; ++i)
			{
				// Load mesh & create material
				ResourcePtr<Mesh> mesh = engine->resourceFactory->Create<Mesh>();
				MeshLoader::LoadFBX(mesh.Get(), node->GetMesh());
				Material* mat = engine->resourceFactory->Create<Material>("Default Phong");

				// Attach renderer
				//Renderer* r = co->AttachComponent<Renderer>();

				// Load & attach textures
				FbxLayerElementMaterial* lE = node->GetGeometry()->GetLayer(i)->GetMaterials();
				switch (lE->GetMappingMode())
				{
				case FbxLayerElement::eAllSame:
					{
						FbxSurfaceMaterial* surf = node->GetMaterial(lE->GetIndexArray()[0]);
						if (surf == nullptr) break;

						FbxProperty prop = surf->FindProperty(FbxSurfaceMaterial::sDiffuse);
						FbxFileTexture* fTex = prop.GetSrcObject<FbxFileTexture>();
						if (fTex == nullptr) break;

						std::string name = fTex->GetFileName();
						Texture* tex = engine->resourceFactory->Create<Texture>(name);

						Renderer* r = co->AttachComponent<Renderer>();
						r->SetTexture(tex);
						r->Init(mat, mesh.Get());
					}
					//r->Init(mat, mesh);
					break;
				case FbxLayerElement::eByPolygon:
					{
						//Resources::ResourceRef<Resources::TextureArray> texArray = engine->resourceFactory->Create<Resources::TextureArray>();
						int vtxCnt = 0;
						//int matCnt = node->GetMaterialCount();
						int matCnt = node->GetMesh()->GetPolygonCount();
						for (int j = 0; j < matCnt; ++j)
						{
							FbxSurfaceMaterial* surf = node->GetMaterial(lE->GetIndexArray()[j]);
							//node->GetMaterial(j);
							if (surf == nullptr) continue;

							FbxProperty prop = surf->FindProperty(FbxSurfaceMaterial::sDiffuse);
							FbxFileTexture* fTex = prop.GetSrcObject<FbxFileTexture>();
							if (fTex == nullptr) continue;

							std::string name = fTex->GetFileName();
							Texture* tex = engine->resourceFactory->Create<Texture>(name);

							ResourcePtr<TextureArray> texArray = engine->resourceFactory->Create<TextureArray>();
							texArray = texArray->Add(tex);

							Elements::CompositeObject* ro = engine->elementFactory->Create<Elements::CompositeObject>();
							ro->GetComponent<Elements::Transform>()->SetPosition({0.0f, 0.0f, 0.0f});
							ro->GetComponent<Elements::Transform>()->SetRotation({0.0f, 0.0f, 0.0f});
							ro->GetComponent<Elements::Transform>()->SetScale({1.0f, 1.0f, 1.0f});
							ro->GetComponent<Elements::Transform>()->SetParent(co->GetComponent<Elements::Transform>());

							Renderer* r = ro->AttachComponent<Renderer>();

							r->SetTexture(texArray.Get());
							ResourcePtr<Mesh> mm = engine->resourceFactory->Create<Mesh>();

							int polySize = node->GetMesh()->GetPolygonSize(j);
							for (int k = 0; k < polySize; ++k)
							{
								mm->vertices.push_back(mesh->vertices[vtxCnt]);
								++vtxCnt;
							}
							for (int k = 0; k < polySize - 2; ++k)
							{
								mm->indices.push_back(0);
								mm->indices.push_back(k + 1);
								mm->indices.push_back(k + 2);
							}

							r->Init(mat, mm.Get());
						}

						// Update polygon texcoords to reference respective textures
						/*int vtxCnt = 0;
						unsigned polyCnt = node->GetMesh()->GetPolygonCount();
						for (int j = 0; j < polyCnt; ++j)
						{
							int matIdx = lE->GetIndexArray()[j];
							int polySize = node->GetMesh()->GetPolygonSize(j);
							for (int k = 0; k < polySize; ++k)
							{
								mesh->vertices[vtxCnt].tex.z = matIdx;
								++vtxCnt;
							}
						}*/

						//r->SetTexture(*texArray);
					}
					//r->Init(mat, mesh);
					break;
				}
			}
		}
		break;

	case FbxNodeAttribute::eLight:
		{
			FbxLight* light = node->GetLight();
			Graphics::Light* l = co->AttachComponent<Graphics::Light>();
			l->m_shadows = false;
			l->m_colour.x = light->Color.Get().mData[0];
			l->m_colour.y = light->Color.Get().mData[1];
			l->m_colour.z = light->Color.Get().mData[2];

			engine->graphics->AddLight(l);
		}
		break;
	default:
		break;
	}

	return co;
}

Elements::CompositeObject* SceneLoader::LoadFBX(Engine* engine, FbxScene* fbxScene)
{
	return ApplyFBXRecursively(engine, fbxScene->GetRootNode());
}

Elements::CompositeObject* SceneLoader::LoadFBX(Engine* engine, const std::string& filename)
{
	// Create necessary resources
	FbxManager* fbx = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(fbx, IOSROOT);
	fbx->SetIOSettings(ios);
	FbxImporter* fbxImporter = FbxImporter::Create(fbx, "");

	bool success = fbxImporter->Initialize(filename.c_str(), -1, fbx->GetIOSettings());

	if (!success) return nullptr;

	FbxScene* fbxScene = FbxScene::Create(fbx, filename.c_str());
	fbxImporter->Import(fbxScene);
	fbxImporter->Destroy();

	Elements::CompositeObject* co = LoadFBX(engine, fbxScene);

	// Clean up
	fbx->Destroy();

	return co;
}
