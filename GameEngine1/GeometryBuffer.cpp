#include "GeometryBuffer.h"
#include <algorithm>
#include "CompositeObject.h"
#include "Transform.h"
#include "GraphicsController.h"
#include <comdef.h>
#include <iostream>

GeometryBuffer::GeometryBuffer(bool s) : m_static(s) {}

GeometryBuffer::~GeometryBuffer()
{
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

		GraphicsController::instance->device->CreateBuffer(&vBD, NULL, &vertexBuffer);

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

		GraphicsController::instance->device->CreateBuffer(&iBD, NULL, &indexBuffer);

		indexSize = iSize;
	}
}

GeometryBuffer::BufferLocation GeometryBuffer::AddRenderer(Renderer* r)
{
	Resize(
		vertexPos + (*r->GetMesh())[0].vertices.size(),
		indexPos + (*r->GetMesh())[0].indices.size()
	);

	auto vData = &(*r->GetMesh())[0].vertices;
	auto iData = &(*r->GetMesh())[0].indices;
	if (m_static)
	{
		for (auto v : *vData)
			vertexBack.push_back(v);
		for (auto i : *iData)
			indexBack.push_back(i);

		//vData = &vertexBack;
		//iData = &indexBack;
	}

	D3D11_MAPPED_SUBRESOURCE mp;
	GraphicsController::instance->devContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mp);
	memcpy(mp.pData, &(*vData)[0], vData->size() * sizeof(Vertex));
	GraphicsController::instance->devContext->Unmap(vertexBuffer, 0);

	GraphicsController::instance->devContext->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mp);
	memcpy(mp.pData, &(*iData)[0], iData->size() * sizeof(unsigned));
	GraphicsController::instance->devContext->Unmap(indexBuffer, 0);

	GeometryBuffer::BufferLocation ret = std::make_tuple(vertexPos, indexPos);

	vertexPos += (*r->GetMesh())[0].vertices.size();
	indexPos += (*r->GetMesh())[0].indices.size();

	return ret;
}

void GeometryBuffer::Select()
{
	unsigned stride = sizeof(Vertex);
	unsigned offset = 0;
	GraphicsController::instance->devContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	GraphicsController::instance->devContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);
}