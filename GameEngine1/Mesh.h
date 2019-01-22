#ifndef __MESH_H__
#define __MESH_H__

#include "Resource.h"
#include <DirectXMath.h>
#include <vector>

struct Vertex
{
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tex;
};

namespace GameEngine
{
	namespace Resources
	{
		class Mesh : public Resource
		{
		public:
			std::vector<Vertex> vertices;
			std::vector<int> indices;

			bool m_generated = false;
			DirectX::XMFLOAT3 m_centroid;

		protected:
			Mesh* CloneResource();

		public:
			Mesh* CombineMesh(Mesh*);

			DirectX::XMVECTOR GetCentroid();
		};
	}
}

#endif