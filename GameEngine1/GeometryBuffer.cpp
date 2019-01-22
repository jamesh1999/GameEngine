#include "GeometryBuffer.h"
#include <algorithm>
#include "CompositeObject.h"
#include "Transform.h"
#include "GraphicsController.h"
#include <comdef.h>
#include <iostream>

void GeometryBuffer::Push()
{
	Resize(vertexPos, indexPos);

	D3D11_MAPPED_SUBRESOURCE mpV;
	devContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mpV);

	D3D11_MAPPED_SUBRESOURCE mpI;
	devContext->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mpI);

	int vPos = 0;
	int iPos = 0;
	for (GameEngine::Renderer* r : m_renderers)
	{
		auto vData = &r->GetMesh()->vertices;
		auto iData = &r->GetMesh()->indices;

		memcpy(reinterpret_cast<uint8_t*>(mpV.pData) + vPos * sizeof(Vertex),
			&(*vData)[0],
			vData->size() * sizeof(Vertex));
		memcpy(reinterpret_cast<uint8_t*>(mpI.pData) + iPos * sizeof(unsigned),
			&(*iData)[0],
			iData->size() * sizeof(unsigned));

		vPos += r->GetMesh()->vertices.size();
		iPos += r->GetMesh()->indices.size();
	}

	devContext->Unmap(indexBuffer, 0);
	devContext->Unmap(vertexBuffer, 0);

	m_pushed = true;
}

GeometryBuffer::GeometryBuffer(ID3D11Device* d, ID3D11DeviceContext* dc, bool s) : dev(d), devContext(dc), m_static(s)
{
	dev->AddRef();
	devContext->AddRef();
}

GeometryBuffer::~GeometryBuffer()
{
 	dev->Release();
	devContext->Release();
	if (vertexBuffer != nullptr)
		vertexBuffer->Release();
	if (indexBuffer != nullptr)
		indexBuffer->Release();
}

void GeometryBuffer::Resize(int vSize, int iSize)
{
	if (vSize > vertexSize)
	{
		if (vertexBuffer != nullptr) vertexBuffer->Release();

		D3D11_BUFFER_DESC vBD;
		ZeroMemory(&vBD, sizeof(D3D11_BUFFER_DESC));
		vBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vBD.ByteWidth = sizeof(Vertex) * vSize;
		vBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vBD.Usage = D3D11_USAGE_DYNAMIC;

		dev->CreateBuffer(&vBD, NULL, &vertexBuffer);

		vertexSize = vSize;
	}

	if (iSize > indexSize)
	{
		if (indexBuffer != nullptr) indexBuffer->Release();

		D3D11_BUFFER_DESC iBD;
		ZeroMemory(&iBD, sizeof(D3D11_BUFFER_DESC));
		iBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
		iBD.ByteWidth = sizeof(unsigned) * iSize;
		iBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		iBD.Usage = D3D11_USAGE_DYNAMIC;

		dev->CreateBuffer(&iBD, NULL, &indexBuffer);

		indexSize = iSize;
	}
}

GeometryBuffer::BufferLocation GeometryBuffer::AddRenderer(GameEngine::Renderer* r)
{
	m_pushed = false;
	m_renderers.push_back(r);

	GeometryBuffer::BufferLocation ret = std::make_tuple(vertexPos, indexPos);

	vertexPos += r->GetMesh()->vertices.size();
	indexPos += r->GetMesh()->indices.size();

	return ret;
}

void GeometryBuffer::Select()
{
	if (!m_pushed) Push();

	unsigned stride = sizeof(Vertex);
	unsigned offset = 0;
	devContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	devContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}