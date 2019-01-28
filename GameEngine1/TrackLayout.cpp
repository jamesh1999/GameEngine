#include "TrackLayout.h"
#include <cmath>
#include <iostream>

TrackLayout* TrackLayout::instance = nullptr;

TrackLayout::TrackLayout()
{
	instance = this;
}

void TrackLayout::SetTrack(GameEngine::Resources::Mesh* mesh)
{
	instance->m_trackSegments.clear();

	Segment s;
	for (int i = 0; i < mesh->indices.size(); i += 3)
	{
		s.p1 = mesh->vertices[mesh->indices[i]].pos;
		s.p2 = mesh->vertices[mesh->indices[i + 1]].pos;
		s.p3 = mesh->vertices[mesh->indices[i + 2]].pos;

		s.n1 = mesh->vertices[mesh->indices[i]].normal;
		s.n2 = mesh->vertices[mesh->indices[i + 1]].normal;
		s.n3 = mesh->vertices[mesh->indices[i + 2]].normal;

		DirectX::XMVECTOR p1 = DirectX::XMLoadFloat3(&s.p1);
		DirectX::XMVECTOR p2 = DirectX::XMLoadFloat3(&s.p2);
		DirectX::XMVECTOR p3 = DirectX::XMLoadFloat3(&s.p3);

		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(p2, p1);
		DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(p3, p1);

		DirectX::XMVECTOR norm = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(v1, v2));
		DirectX::XMStoreFloat3(&s.norm, norm);
		s.pos = DirectX::XMVectorGetX(DirectX::XMVector3Dot(norm, p1));

		instance->m_trackSegments.push_back(s);
	}

}

std::tuple<DirectX::XMVECTOR, DirectX::XMVECTOR, bool> TrackLayout::GetNormal(DirectX::XMVECTOR pos)
{
	float minDist = FLT_MAX;
	DirectX::XMFLOAT3 shipNorm = { 0.0f, 1.0f, 0.0f };
	DirectX::XMVECTOR projFinal = { 0.0f, 0.0f, 0.0f };
	bool over = false;
	for (Segment s : instance->m_trackSegments)
	{
		DirectX::XMVECTOR norm = DirectX::XMLoadFloat3(&s.norm);
		DirectX::XMVECTOR vdist = DirectX::XMVectorSubtract(DirectX::XMVector3Dot(norm, pos), DirectX::XMVector3Dot(norm, DirectX::XMLoadFloat3(&s.p1)));
		DirectX::XMVECTOR proj = DirectX::XMVectorSubtract(
			pos,
			DirectX::XMVectorMultiply(norm, vdist));

		DirectX::XMVECTOR v0 = DirectX::XMVectorSubtract(
			DirectX::XMLoadFloat3(&s.p2),
			DirectX::XMLoadFloat3(&s.p1)
		);

		DirectX::XMVECTOR v1 = DirectX::XMVectorSubtract(
			DirectX::XMLoadFloat3(&s.p3),
			DirectX::XMLoadFloat3(&s.p1)
		);

		DirectX::XMVECTOR v2 = DirectX::XMVectorSubtract(
			proj,
			DirectX::XMLoadFloat3(&s.p1)
		);

		float d00 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v0, v0));
		float d01 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v0, v1));
		float d11 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v1, v1));
		float d20 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v2, v0));
		float d21 = DirectX::XMVectorGetX(DirectX::XMVector3Dot(v2, v1));
		float denom = d00 * d11 - d01 * d01;
		float v = (d11 * d20 - d01 * d21) / denom;
		float w = (d00 * d21 - d01 * d20) / denom;


		float dist = std::abs(DirectX::XMVectorGetX(vdist) - s.pos);
		if (v < 0.0f
			|| w < 0.0f
			|| v + w > 1.0f)
			continue;
		if (dist > minDist) continue;
		minDist = dist;

		DirectX::XMStoreFloat3(&shipNorm, 
			DirectX::XMVector3Normalize(
				DirectX::XMVectorAdd(
					DirectX::XMVectorAdd(
						DirectX::XMVectorScale(DirectX::XMLoadFloat3(&s.n1), 1.0f - w - v),
						DirectX::XMVectorScale(DirectX::XMLoadFloat3(&s.n2), v)),
					DirectX::XMVectorScale(DirectX::XMLoadFloat3(&s.n3), w))));

		projFinal = proj;
		over = true;
	}

	return std::make_tuple(DirectX::XMLoadFloat3(&shipNorm), projFinal, over);
}