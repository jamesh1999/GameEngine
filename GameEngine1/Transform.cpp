#include "Transform.h"
#include <cmath>

DirectX::XMMATRIX Transform::GetTransform() const
{
	if(m_parent != nullptr)
		return DirectX::XMMatrixMultiply(
		DirectX::XMMatrixMultiply(
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_rotation)),
			DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&m_scale))),
		DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&m_position))),
		m_parent->GetTransform());
	else
		return DirectX::XMMatrixMultiply(
			DirectX::XMMatrixMultiply(
				DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4(&m_rotation)),
				DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3(&m_scale))),
			DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3(&m_position)));
}

DirectX::XMVECTOR Transform::GetPosition() const
{
	if(m_parent != nullptr)
		return DirectX::XMVector3TransformCoord(
			DirectX::XMLoadFloat3A(&m_position),
			m_parent->GetTransform());
	else
		return DirectX::XMLoadFloat3A(&m_position);
}

DirectX::XMVECTOR Transform::GetRotation() const
{
	if (m_parent != nullptr)
		return DirectX::XMQuaternionMultiply(
			DirectX::XMLoadFloat4A(&m_rotation),
			m_parent->GetRotation());
	else
		return DirectX::XMLoadFloat4A(&m_rotation);
}

DirectX::XMVECTOR Transform::GetScale() const
{
	if (m_parent != nullptr)
		return DirectX::XMVector3TransformNormal(
			DirectX::XMLoadFloat3A(&m_position),
			m_parent->GetTransform());
	else
		return DirectX::XMLoadFloat3A(&m_scale);
}

DirectX::XMVECTOR Transform::GetUp() const
{
	return DirectX::XMVector3Rotate({ 0.0f, 1.0f, 0.0f }, GetRotation());
}

DirectX::XMVECTOR Transform::GetDown() const
{
	return DirectX::XMVector3Rotate({ 0.0f, -1.0f, 0.0f }, GetRotation());
}

DirectX::XMVECTOR Transform::GetLeft() const
{
	return DirectX::XMVector3Rotate({ -1.0f, 0.0f, 0.0f }, GetRotation());
}

DirectX::XMVECTOR Transform::GetRight() const
{
	return DirectX::XMVector3Rotate({ 1.0f, 0.0f, 0.0f }, GetRotation());
}

DirectX::XMVECTOR Transform::GetForwards() const
{
	return DirectX::XMVector3Rotate({ 0.0f, 0.0f, 1.0f }, GetRotation());
}

DirectX::XMVECTOR Transform::GetBackwards() const
{
	return DirectX::XMVector3Rotate({ 0.0f, 0.0f, -1.0f }, GetRotation());
}

DirectX::XMMATRIX Transform::GetLocalTransform() const
{
	return DirectX::XMMatrixMultiply(
		DirectX::XMMatrixMultiply(
			DirectX::XMMatrixRotationQuaternion(DirectX::XMLoadFloat4A(&m_rotation)),
			DirectX::XMMatrixScalingFromVector(DirectX::XMLoadFloat3A(&m_scale))),
		DirectX::XMMatrixTranslationFromVector(DirectX::XMLoadFloat3A(&m_position)));
}

DirectX::XMVECTOR Transform::GetLocalPosition() const
{
	return DirectX::XMLoadFloat3A(&m_position);
}

DirectX::XMVECTOR Transform::GetLocalRotation() const
{
	return DirectX::XMLoadFloat4A(&m_rotation);
}

DirectX::XMVECTOR Transform::GetLocalScale() const
{
	return DirectX::XMLoadFloat3A(&m_scale);
}

void Transform::SetPosition(const DirectX::XMVECTOR& nPos)
{
	DirectX::XMStoreFloat3A(&m_position, nPos);
}

void Transform::SetRotation(const DirectX::XMVECTOR& nRotation)
{
	DirectX::XMStoreFloat4A(&m_rotation, nRotation);
	//Clamp [-pi:pi]
	/*m_rotation.x -= DirectX::XM_2PI * (std::floor((m_rotation.x + DirectX::XM_PI) / DirectX::XM_2PI));
	m_rotation.y -= DirectX::XM_2PI * (std::floor((m_rotation.y + DirectX::XM_PI) / DirectX::XM_2PI));
	m_rotation.z -= DirectX::XM_2PI * (std::floor((m_rotation.z + DirectX::XM_PI) / DirectX::XM_2PI));*/
}

void Transform::SetScale(const DirectX::XMVECTOR& nScale)
{
	DirectX::XMStoreFloat3A(&m_scale, nScale);
}

void Transform::SetParent(Transform* parent)
{
	if (m_parent != nullptr)
	{
		auto it = std::find(m_parent->m_children.begin(), m_parent->m_children.end(), this);
		m_parent->m_children.erase(it);
	}

	m_parent = parent;
	
	if (parent != nullptr) parent->m_children.push_back(this);
}
