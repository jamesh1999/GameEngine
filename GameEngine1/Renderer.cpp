#include "Renderer.h"
#include "GraphicsController.h"
#include "ResourceFactory.h"

using namespace GameEngine;

void Renderer::Destroy()
{
	if (m_init) engine->graphics->RemoveRenderer(this);
	Component::Destroy();
}

void Renderer::Render(int i)
{
	GeometryBuffer::BufferLocation idxes = engine->graphics->geomBuff->FindRenderer(this);

	if (m_textures.Get() == nullptr) return;
	ID3D11ShaderResourceView* srv = m_textures->GetSRV();
	engine->graphics->devContext->PSSetShaderResources(0, 1, &srv);

	for (size_t j = 0; j < mat->passes.size(); ++j)
	{
		engine->graphics->devContext->IASetInputLayout(mat->passes[j].layout);
		engine->graphics->devContext->VSSetShader(mat->passes[j].vs, nullptr, 0);
		engine->graphics->devContext->PSSetShader(mat->passes[j].ps, nullptr, 0);

		engine->graphics->devContext->DrawIndexed(mesh->indices.size(), std::get<0>(idxes), std::get<1>(idxes));
	}
}

void Renderer::Init(Material* material, Resources::Mesh* m)
{
	mat = material;
	mesh = m;
	engine->graphics->AddRenderer(this);
	m_init = true;
}

void Renderer::SetMaterial(Material* m)
{
	mat = m;
}

void Renderer::SetMesh(Resources::Mesh* m)
{
	mesh = m;
}

void Renderer::SetTexture(Resources::TextureArray* texArray)
{
	m_textures = texArray;
}

void Renderer::SetTexture(Resources::Texture* tex)
{
	m_textures = engine->resourceFactory->Create<Resources::TextureArray>();
	m_textures = m_textures->Add(tex);
}

Resources::Mesh* Renderer::GetMesh()
{
	//return *mesh;
	return mesh;
}

bool Renderer::GetTransparent()
{
	return m_textures->GetTransparent();
}