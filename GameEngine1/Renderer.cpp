#include "Renderer.h"
#include "GraphicsController.h"

void Renderer::Render(int i)
{
	int min = mesh->size();
	if (mat.Size() < min)
		min = mat.Size();

	GeometryBuffer::BufferLocation idxes = engine->graphics->geomBuff->FindRenderer(this);

	if (i < min)
	{
		for (int j = 0; j < mat[i]->passes.size(); ++j)
		{
			engine->graphics->devContext->IASetInputLayout(mat[i]->passes[j].layout);
			engine->graphics->devContext->VSSetShader(mat[i]->passes[j].vs, NULL, NULL);
			engine->graphics->devContext->PSSetShader(mat[i]->passes[j].ps, NULL, NULL);

			engine->graphics->devContext->DrawIndexed((*mesh)[i].indices.size(), std::get<0>(idxes), std::get<1>(idxes));
		}
	}
}

void Renderer::Init(MaterialGroup material, MeshData* m)
{
	mat = material;
	mesh = m;
	engine->graphics->AddRenderer(this);
}

Renderer::~Renderer()
{
	delete mesh;
}

void Renderer::SetMaterial(MaterialGroup m)
{
	mat = m;
}

void Renderer::SetMesh(MeshData* m)
{
	delete mesh;
	mesh = m;
}

MeshData* Renderer::GetMesh()
{
	return mesh;
}