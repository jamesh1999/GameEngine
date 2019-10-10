#include "StaticBatcher.h"
#include "Component.h"
#include "ElementFactory.h"
#include "ElementPtr.h"
#include "GraphicsController.h"
#include "Mesh.h"
#include "ResourceFactory.h"
#include "ResourcePtr.h"
#include "TextureArray.h"
#include "Transform.h"

// Recursively find all renderers beneath a point
void StaticBatcher::AggregateRenderers(GameEngine::Elements::CompositeObject* co, std::vector<GameEngine::Renderer*>& agg)
{
	GameEngine::Renderer* r = co->GetComponent<GameEngine::Renderer>();
	if (r != nullptr) agg.push_back(r);

	for (GameEngine::Elements::ElementPtr<GameEngine::Elements::Transform> child :
		co->GetComponent<GameEngine::Elements::Transform>()->GetChildren())
		AggregateRenderers(child->obj.Get(), agg);
}

// Merge all renderers below a point into a single renderer at the point
void StaticBatcher::BatchFrom(GameEngine::Elements::CompositeObject* root, GameEngine::Engine* engine)
{
	// Find all renderers below root
	std::vector<GameEngine::Renderer*> renderers;
	AggregateRenderers(root, renderers);


	bool unbatched = true;
	while (unbatched)
	{
		unbatched = false;
		GameEngine::Elements::CompositeObject* trans = 
			engine->elementFactory->Create<GameEngine::Elements::CompositeObject>();
		trans->GetComponent<GameEngine::Elements::Transform>()->SetParent(root->GetComponent<GameEngine::Elements::Transform>());
		trans->GetComponent<GameEngine::Elements::Transform>()->SetScale({1.0f, 1.0f, 1.0f});
		trans->GetComponent<GameEngine::Elements::Transform>()->SetPosition({0.0f, 0.0f, 0.0f});
		trans->GetComponent<GameEngine::Elements::Transform>()->SetRotation({0.0f, 0.0f, 0.0f});

		GameEngine::Resources::ResourcePtr<GameEngine::Resources::Mesh> m = engine->resourceFactory->Create<GameEngine::Resources::Mesh>();
		GameEngine::Renderer* tr = trans->AttachComponent<GameEngine::Renderer>();
		auto ta = engine->resourceFactory->Create<GameEngine::Resources::TextureArray>();

		Material* mat = nullptr;
		int idxOffset = 0;
		for (auto r : renderers)
		{
			if (r->GetTransparent()) continue;
			if (!r->m_active) continue;
			if (mat == nullptr) mat = r->mat.Get();
			if (r->mat.Get() != mat)
			{
				unbatched = true;
				continue;
			}
			r->m_active = false;

			int* map = new int[r->m_textures->Size()];
			for (int i = 0; i < r->m_textures->Size(); ++i)
			{
				map[i] = ta->Find((*r->m_textures)[i]);
				if (map[i] != -1) continue;
				ta = ta->Add((*r->m_textures)[i]);
				map[i] = ta->Size() - 1;
			}

			for (size_t i = 0; i < r->mesh->vertices.size(); ++i)
			{
				Vertex v = r->mesh->vertices[i];
				v.tex.z = static_cast<float>(map[static_cast<int>(v.tex.z)]);
				m->vertices.push_back(v);
			}

			for (size_t i = 0; i < r->mesh->indices.size(); ++i)
				m->indices.push_back(r->mesh->indices[i] + idxOffset);

			idxOffset += r->mesh->vertices.size();

			delete[] map;
		}

		tr->m_textures = ta;
		tr->Init(mat, m.Get());
	}

	unbatched = true;
	while (unbatched)
	{
		unbatched = false;
		GameEngine::Elements::CompositeObject* trans = engine
		                                               ->elementFactory->Create<GameEngine::Elements::CompositeObject
		                                               >();
		trans->GetComponent<GameEngine::Elements::Transform>()->SetParent(
			root->GetComponent<GameEngine::Elements::Transform>());
		trans->GetComponent<GameEngine::Elements::Transform>()->SetScale({1.0f, 1.0f, 1.0f});
		trans->GetComponent<GameEngine::Elements::Transform>()->SetPosition({0.0f, 0.0f, 0.0f});
		trans->GetComponent<GameEngine::Elements::Transform>()->SetRotation({0.0f, 0.0f, 0.0f});

		GameEngine::Resources::ResourcePtr<GameEngine::Resources::Mesh> m = engine->resourceFactory->Create<GameEngine::Resources::Mesh>();
		GameEngine::Renderer* tr = trans->AttachComponent<GameEngine::Renderer>();
		auto ta = engine->resourceFactory->Create<GameEngine::Resources::TextureArray>();

		Material* mat = nullptr;
		int idxOffset = 0;
		for (auto r : renderers)
		{
			if (!r->m_active) continue;
			if (mat == nullptr) mat = r->mat.Get();
			if (r->mat.Get() != mat)
			{
				unbatched = true;
				continue;
			}
			r->m_active = false;

			int* map = new int[r->m_textures->Size()];
			for (int i = 0; i < r->m_textures->Size(); ++i)
			{
				map[i] = ta->Find((*r->m_textures)[i]);
				if (map[i] != -1) continue;
				ta = ta->Add((*r->m_textures)[i]);
				map[i] = ta->Size() - 1;
			}

			for (size_t i = 0; i < r->mesh->vertices.size(); ++i)
			{
				Vertex v = r->mesh->vertices[i];
				v.tex.z = static_cast<float>(map[static_cast<int>(v.tex.z)]);
				m->vertices.push_back(v);
			}

			for (size_t i = 0; i < r->mesh->indices.size(); ++i)
				m->indices.push_back(r->mesh->indices[i] + idxOffset);

			idxOffset += r->mesh->vertices.size();

			delete[] map;
		}

		tr->m_textures = ta;
		tr->Init(mat, m.Get());
	}

	// Destroy all batched renderers
	for (auto r : renderers)
		r->Destroy();
}

// Recursively destroy all empty objects
void StaticBatcher::CullHeirarchy(GameEngine::Elements::CompositeObject* root)
{
	for (auto child : root->GetComponent<GameEngine::Elements::Transform>()->GetChildren())
		CullHeirarchy(child->obj.Get());

	if (root->GetComponent<GameEngine::Elements::Transform>()->GetChildren().size()) return;
	if (root->m_components.size()) return;
	root->Destroy();
}
