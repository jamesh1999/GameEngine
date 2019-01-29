#ifndef __TRANSFORM_INCLUDED__
#define __TRANSFORM_INCLUDED__

#include "Component.h"
#include "DirectXMath.h"
#include "ElementPtr.h"
#include <vector>
#include <unordered_set>

namespace GameEngine
{
	namespace Elements
	{
		class Transform : public Component
		{
		private:

			DirectX::XMFLOAT3A m_position;
			DirectX::XMFLOAT4A m_rotation;
			DirectX::XMFLOAT3A m_scale;

			ElementPtr<Transform> m_parent;
			std::unordered_set<ElementPtr<Transform>> m_children;

		public:
			bool m_static = false;

			void Destroy() override;

			DirectX::XMMATRIX GetTransform() const;
			DirectX::XMVECTOR GetPosition() const;
			DirectX::XMVECTOR GetRotation() const;
			DirectX::XMVECTOR GetScale() const;

			DirectX::XMVECTOR GetUp() const;
			DirectX::XMVECTOR GetDown() const;
			DirectX::XMVECTOR GetLeft() const;
			DirectX::XMVECTOR GetRight() const;
			DirectX::XMVECTOR GetForwards() const;
			DirectX::XMVECTOR GetBackwards() const;

			DirectX::XMMATRIX GetLocalTransform() const;
			DirectX::XMVECTOR GetLocalPosition() const;
			DirectX::XMVECTOR GetLocalRotation() const;
			DirectX::XMVECTOR GetLocalScale() const;

			void SetPosition(const DirectX::XMVECTOR&);
			void SetRotation(const DirectX::XMVECTOR&);
			void SetScale(const DirectX::XMVECTOR&);

			std::vector<ElementPtr<Transform>> GetChildren();

			void SetParent(Transform*);
		};
	}
}

#endif
