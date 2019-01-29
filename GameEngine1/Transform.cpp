#include "Transform.h"
#include "CompositeObject.h"
#include <cmath>
#include "Component.h"

using namespace GameEngine::Elements;

void Transform::Destroy()
{
	while (m_children.size())
		(*m_children.begin())->obj->Destroy();

	if (m_parent != nullptr)
	{
		m_parent->m_children.erase(this);
	}

	Element::Destroy();
}

DirectX::XMMATRIX Transform::GetTransform() const
{
	if (m_parent != nullptr)
		return XMMatrixMultiply(
			XMMatrixMultiply(
				XMMatrixMultiply(
					DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&m_rotation)),
					DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&m_scale))),
				DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&m_position))),
			m_parent->GetTransform());
	return XMMatrixMultiply(
		XMMatrixMultiply(
			DirectX::XMMatrixRotationQuaternion(XMLoadFloat4(&m_rotation)),
			DirectX::XMMatrixScalingFromVector(XMLoadFloat3(&m_scale))),
		DirectX::XMMatrixTranslationFromVector(XMLoadFloat3(&m_position)));
}

DirectX::XMVECTOR Transform::GetPosition() const
{
	if (m_parent != nullptr)
		return XMVector3TransformCoord(
			XMLoadFloat3A(&m_position),
			m_parent->GetTransform());
	return XMLoadFloat3A(&m_position);
}

DirectX::XMVECTOR Transform::GetRotation() const
{
	if (m_parent != nullptr)
		return DirectX::XMQuaternionMultiply(
			XMLoadFloat4A(&m_rotation),
			m_parent->GetRotation());
	return XMLoadFloat4A(&m_rotation);
}

DirectX::XMVECTOR Transform::GetScale() const
{
	if (m_parent != nullptr)
		return XMVector3TransformNormal(
			XMLoadFloat3A(&m_position),
			m_parent->GetTransform());
	return XMLoadFloat3A(&m_scale);
}

DirectX::XMVECTOR Transform::GetUp() const
{
	return DirectX::XMVector3Rotate({0.0f, 1.0f, 0.0f}, GetRotation());
}

DirectX::XMVECTOR Transform::GetDown() const
{
	return DirectX::XMVector3Rotate({0.0f, -1.0f, 0.0f}, GetRotation());
}

DirectX::XMVECTOR Transform::GetLeft() const
{
	return DirectX::XMVector3Rotate({-1.0f, 0.0f, 0.0f}, GetRotation());
}

DirectX::XMVECTOR Transform::GetRight() const
{
	return DirectX::XMVector3Rotate({1.0f, 0.0f, 0.0f}, GetRotation());
}

DirectX::XMVECTOR Transform::GetForwards() const
{
	return DirectX::XMVector3Rotate({0.0f, 0.0f, 1.0f}, GetRotation());
}

DirectX::XMVECTOR Transform::GetBackwards() const
{
	return DirectX::XMVector3Rotate({0.0f, 0.0f, -1.0f}, GetRotation());
}

DirectX::XMMATRIX Transform::GetLocalTransform() const
{
	return XMMatrixMultiply(
		XMMatrixMultiply(
			DirectX::XMMatrixRotationQuaternion(XMLoadFloat4A(&m_rotation)),
			DirectX::XMMatrixScalingFromVector(XMLoadFloat3A(&m_scale))),
		DirectX::XMMatrixTranslationFromVector(XMLoadFloat3A(&m_position)));
}

DirectX::XMVECTOR Transform::GetLocalPosition() const
{
	return XMLoadFloat3A(&m_position);
}

DirectX::XMVECTOR Transform::GetLocalRotation() const
{
	return XMLoadFloat4A(&m_rotation);
}

DirectX::XMVECTOR Transform::GetLocalScale() const
{
	return XMLoadFloat3A(&m_scale);
}

void Transform::SetPosition(const DirectX::XMVECTOR& nPos)
{
	XMStoreFloat3A(&m_position, nPos);
}

void Transform::SetRotation(const DirectX::XMVECTOR& nRotation)
{
	XMStoreFloat4A(&m_rotation, nRotation);
	//Clamp [-pi:pi]
	/*m_rotation.x -= DirectX::XM_2PI * (std::floor((m_rotation.x + DirectX::XM_PI) / DirectX::XM_2PI));
	m_rotation.y -= DirectX::XM_2PI * (std::floor((m_rotation.y + DirectX::XM_PI) / DirectX::XM_2PI));
	m_rotation.z -= DirectX::XM_2PI * (std::floor((m_rotation.z + DirectX::XM_PI) / DirectX::XM_2PI));*/
}

void Transform::SetScale(const DirectX::XMVECTOR& nScale)
{
	XMStoreFloat3A(&m_scale, nScale);
}

std::vector<ElementPtr<Transform>> Transform::GetChildren()
{
	return std::vector<ElementPtr<Transform>>(m_children.begin(), m_children.end());
}

void Transform::SetParent(Transform* parent)
{
	if (m_parent != nullptr)
	{
		m_parent->m_children.erase(this);
	}

	m_parent = parent;

	if (parent != nullptr) parent->m_children.emplace(this);
}
