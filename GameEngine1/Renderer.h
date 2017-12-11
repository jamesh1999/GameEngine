#ifndef __GRAPHICS_OBJECT_INCLUDED___
#define __GRAPHICS_OBJECT_INCLUDED___

#include <DirectXMath.h>
#include <vector>
#include "Component.h"
#include "Material.h"
#include "MeshData.h"

class Renderer : public GameEngine::ObjectSystem::Component
{
	//friend class GraphicsController;

public:
	MaterialGroup mat;
	MeshData* mesh;

public:


	void Render(int);

	Renderer() = default;
	~Renderer();

	Renderer(const Renderer&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(const Renderer&) = delete;
	Renderer& operator=(Renderer&&) = delete;

	void Init(MaterialGroup mat, MeshData* mesh);

	void SetMesh(MeshData*);
	void SetMaterial(MaterialGroup);

	MeshData* GetMesh();
};

#endif