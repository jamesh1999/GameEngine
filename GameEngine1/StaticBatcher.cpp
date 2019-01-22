#include "StaticBatcher.h"
#include "Transform.h"
#include "Mesh.h"
#include "ResourceFactory.h"
#include "TextureArray.h"
#include "ResourceRef.h"
#include "ElementFactory.h"
#include "GraphicsController.h"

void StaticBatcher::AggregateRenderers(GameEngine::Elements::CompositeObject* co, std::vector<GameEngine::Renderer*>& agg)
{
	GameEngine::Renderer* r = co->GetComponent<GameEngine::Renderer>();
	if (r != nullptr) agg.push_back(r);

	for (Transform* child : co->GetComponent<Transform>()->m_children)
		AggregateRenderers(child->obj, agg);
}

void StaticBatcher::BatchFrom(GameEngine::Elements::CompositeObject* root, GameEngine::Engine* engine)
{
	std::vector<GameEngine::Renderer*> renderers;
	AggregateRenderers(root, renderers);

	/*GameEngine::Renderer* nr = root->AttachComponent<GameEngine::Renderer>();

	auto m = new GameEngine::Resources::Mesh;
	GameEngine::Resources::ResourceRef<GameEngine::Resources::TextureArray> ta = engine->resourceFactory->Create<GameEngine::Resources::TextureArray>();

	int idxOffset = 0;
	for (auto r : renderers)
	{
		if (r->GetTransparent()) continue;
		r->m_active = false;

		int* map = new int[r->m_textures->Size()];
		for (int i = 0; i < r->m_textures->Size(); ++i)
		{
			map[i] = ta->Find((**r->m_textures)[i]);
			if (map[i] != -1) continue;
			ta = ta->Add((**r->m_textures)[i]);
			map[i] = ta->Size() - 1;
		}

		for (int i = 0; i < r->mesh->vertices.size(); ++i)
		{
			Vertex v = r->mesh->vertices[i];
			v.tex.z = map[static_cast<int>(v.tex.z)];
			m->vertices.push_back(v);
		}

		for (int i = 0; i < r->mesh->indices.size(); ++i)
			m->indices.push_back(r->mesh->indices[i] + idxOffset);

		idxOffset += r->mesh->vertices.size();

		delete[] map;
	}

	nr->m_textures = ta;
	nr->Init(*renderers[0]->mat, m);*/

	bool unbatched = true;
	while (unbatched) {
		unbatched = false;
		GameEngine::Elements::CompositeObject* trans = engine->elementFactory->Create<GameEngine::Elements::CompositeObject>();
		trans->GetComponent<Transform>()->SetParent(root->GetComponent<Transform>());
		trans->GetComponent<Transform>()->SetScale({ 1.0f, 1.0f, 1.0f });
		trans->GetComponent<Transform>()->SetPosition({ 0.0f, 0.0f, 0.0f });
		trans->GetComponent<Transform>()->SetRotation({ 0.0f, 0.0f, 0.0f });

		auto m = new GameEngine::Resources::Mesh;
		GameEngine::Renderer* tr = trans->AttachComponent<GameEngine::Renderer>();
		auto ta = engine->resourceFactory->Create<GameEngine::Resources::TextureArray>();

		Material* mat = nullptr;
		int idxOffset = 0;
		for (auto r : renderers)
		{
			if (r->GetTransparent()) continue;
			if (!r->m_active) continue;
			if (mat == nullptr) mat = *r->mat;
			if (*r->mat != mat) { unbatched = true; continue; }
			r->m_active = false;

			int* map = new int[r->m_textures->Size()];
			for (int i = 0; i < r->m_textures->Size(); ++i)
			{
				map[i] = ta->Find((**r->m_textures)[i]);
				if (map[i] != -1) continue;
				ta = ta->Add((**r->m_textures)[i]);
				map[i] = ta->Size() - 1;
			}

			for (int i = 0; i < r->mesh->vertices.size(); ++i)
			{
				Vertex v = r->mesh->vertices[i];
				v.tex.z = map[static_cast<int>(v.tex.z)];
				m->vertices.push_back(v);
			}

			for (int i = 0; i < r->mesh->indices.size(); ++i)
				m->indices.push_back(r->mesh->indices[i] + idxOffset);

			idxOffset += r->mesh->vertices.size();

			delete[] map;
		}

		tr->m_textures = ta;
		tr->Init(mat, m);
	}

	unbatched = true;
	while (unbatched) {
		unbatched = false;
		GameEngine::Elements::CompositeObject* trans = engine->elementFactory->Create<GameEngine::Elements::CompositeObject>();
		trans->GetComponent<Transform>()->SetParent(root->GetComponent<Transform>());
		trans->GetComponent<Transform>()->SetScale({ 1.0f, 1.0f, 1.0f });
		trans->GetComponent<Transform>()->SetPosition({ 0.0f, 0.0f, 0.0f });
		trans->GetComponent<Transform>()->SetRotation({ 0.0f, 0.0f, 0.0f });

		auto m = new GameEngine::Resources::Mesh;
		GameEngine::Renderer* tr = trans->AttachComponent<GameEngine::Renderer>();
		auto ta = engine->resourceFactory->Create<GameEngine::Resources::TextureArray>();

		Material* mat = nullptr;
		int idxOffset = 0;
		for (auto r : renderers)
		{
			if (!r->m_active) continue;
			if (mat == nullptr) mat = *r->mat;
			if (*r->mat != mat) { unbatched = true; continue; }
			r->m_active = false;

			int* map = new int[r->m_textures->Size()];
			for (int i = 0; i < r->m_textures->Size(); ++i)
			{
				map[i] = ta->Find((**r->m_textures)[i]);
				if (map[i] != -1) continue;
				ta = ta->Add((**r->m_textures)[i]);
				map[i] = ta->Size() - 1;
			}

			for (int i = 0; i < r->mesh->vertices.size(); ++i)
			{
				Vertex v = r->mesh->vertices[i];
				v.tex.z = map[static_cast<int>(v.tex.z)];
				m->vertices.push_back(v);
			}

			for (int i = 0; i < r->mesh->indices.size(); ++i)
				m->indices.push_back(r->mesh->indices[i] + idxOffset);

			idxOffset += r->mesh->vertices.size();

			delete[] map;
		}

		tr->m_textures = ta;
		tr->Init(mat, m);
	}

	for (auto r : renderers)
		r->Destroy();
}

void StaticBatcher::CullHeirarchy(GameEngine::Elements::CompositeObject* root)
{
	
}