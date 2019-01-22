#ifndef __TRACK_LAYOUT_INCLUDED__
#define __TRACK_LAYOUT_INCLUDED__

#include <DirectXMath.h>
#include <vector>
#include <tuple>
#include "Mesh.h"

typedef struct
{
	DirectX::XMFLOAT3 p1;
	DirectX::XMFLOAT3 p2;
	DirectX::XMFLOAT3 p3;

	DirectX::XMFLOAT3 n1;
	DirectX::XMFLOAT3 n2;
	DirectX::XMFLOAT3 n3;

	DirectX::XMFLOAT3 norm;

	float pos;
} Segment;

class TrackLayout
{
private:
	static TrackLayout* instance;
	std::vector<Segment> m_trackSegments;

public:
	TrackLayout();

	static void SetTrack(GameEngine::Resources::Mesh*);
	static std::tuple<DirectX::XMVECTOR, DirectX::XMVECTOR, bool> GetNormal(DirectX::XMVECTOR);
};

#endif