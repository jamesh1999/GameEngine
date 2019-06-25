#ifndef __MESH_H__
#define __MESH_H__

#include <DirectXMath.h>
#include <vector>
#include <string>
#include "Resource.h"
#include "ISerializable.h"

class Vertex : public GameEngine::Utils::ISerializable
{
public:
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT3 normal;
	DirectX::XMFLOAT3 tex;

	Vertex& operator<<(std::istream&);
	Vertex& operator>>(std::ostream&);
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
			Mesh* CloneResource() override;

		public:
			Mesh* CombineMesh(Mesh*);

			DirectX::XMVECTOR GetCentroid();

			Mesh& operator<<(std::istream&);
			Mesh& operator>>(std::ostream&);
		};
	}
}

#endif
