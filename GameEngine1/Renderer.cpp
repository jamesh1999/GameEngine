#include "Renderer.h"
#include "GraphicsController.h"

void Renderer::Render(int i)
{
	int min = mesh->size();
	if (mat.Size() < min)
		min = mat.Size();

	if (i < min)
	{
		for (int j = 0; j < mat[i]->passes.size(); ++j)
		{
			GraphicsController::instance->devContext->IASetInputLayout(mat[i]->passes[j].layout);
			GraphicsController::instance->devContext->VSSetShader(mat[i]->passes[j].vs, NULL, NULL);
			GraphicsController::instance->devContext->PSSetShader(mat[i]->passes[j].ps, NULL, NULL);

			GraphicsController::instance->devContext->DrawIndexed((*mesh)[i].indices.size(), 0, 0);
		}
	}
}

void Renderer::Init(MaterialGroup material, MeshData* m)
{
	mat = material;
	mesh = m;
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