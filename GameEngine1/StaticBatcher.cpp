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

	GameEngine::Renderer* nr = root->AttachComponent<GameEngine::Renderer>();

	auto m = new GameEngine::Resources::Mesh;
	GameEngine::Resources::ResourceRef<GameEngine::Resources::TextureArray> ta = engine->resourceFactory->Create<GameEngine::Resources::TextureArray>();

	int texOffset = 0;
	int idxOffset = 0;
	for (auto r : renderers)
	{
		if (r->GetTransparent()) continue;

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
		texOffset += r->m_textures->Size();

		delete[] map;
	}

	for (auto r : renderers)
	{
		if (!r->GetTransparent()) continue;

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
		texOffset += r->m_textures->Size();

		delete[] map;
	}

	nr->m_textures = ta;
	nr->Init(*renderers[0]->mat, m);

	for (auto r : renderers)
	{
		engine->graphics->RemoveRenderer(r);
		r->Destroy();
	}
}

void StaticBatcher::CullHeirarchy(GameEngine::Elements::CompositeObject* root)
{
	
}