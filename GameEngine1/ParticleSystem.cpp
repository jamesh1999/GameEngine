#include "ParticleSystem.h"
#include "GraphicsController.h"
#include "Clock.h"
#include "TextureLoader.h"
#include "CompositeObject.h"
#include "Transform.h"
#include <iostream>
#include "Texture.h"

using namespace GameEngine::Graphics;

ParticleSystem::ParticleSystem(GameEngine::Engine* engine)
{
	this->engine = engine;

	t = Resources::TextureLoader::LoadTexture("Z:\\WipeoutPSP\\PS2\\Data\\Psys\\Tex\\explosion_01_64x64.tga");
	t->h = 64;
	t->w = 64;

	D3D11_TEXTURE2D_DESC tD;
	ZeroMemory(&tD, sizeof(D3D11_TEXTURE2D_DESC));
	tD.Usage = D3D11_USAGE_IMMUTABLE;
	tD.ArraySize = 1;
	tD.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tD.Height = t->h;
	tD.Width = t->w;
	tD.MiscFlags = 0;
	tD.MipLevels = 1;
	tD.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	tD.SampleDesc.Count = 1;
	tD.SampleDesc.Quality = 0;

	D3D11_SUBRESOURCE_DATA texData;
		texData.SysMemSlicePitch = 4 * 4 * t->h * t->w;
		texData.SysMemPitch = 4 * 4 * t->w;
		texData.pSysMem = t->data;

	engine->graphics->device->CreateTexture2D(&tD, &texData, &m_tex);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvD;
	srvD.Format = tD.Format;
	srvD.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvD.Texture2D.MipLevels = -1;
	srvD.Texture2D.MostDetailedMip = 0;
	engine->graphics->device->CreateShaderResourceView(m_tex, NULL, &m_texView);

	D3D11_BUFFER_DESC vBD;
	ZeroMemory(&vBD, sizeof(D3D11_BUFFER_DESC));
	vBD.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vBD.ByteWidth = sizeof(Vertex) * 300 * 4;
	vBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vBD.Usage = D3D11_USAGE_DYNAMIC;

	engine->graphics->device->CreateBuffer(&vBD, NULL, &vertexBuffer);

	ZeroMemory(&vBD, sizeof(D3D11_BUFFER_DESC));
	vBD.BindFlags = D3D11_BIND_INDEX_BUFFER;
	vBD.ByteWidth = sizeof(Vertex) * 300 * 6;
	vBD.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vBD.Usage = D3D11_USAGE_DYNAMIC;

	engine->graphics->device->CreateBuffer(&vBD, NULL, &indexBuffer);

	//HACK: Need to refactor
	ID3D10Blob *buff = nullptr, *err = nullptr;
	HRESULT success = D3DCompileFromFile(
		L"shaders_particle.hlsl",
		NULL,
		NULL,
		"VShader",
		"vs_5_0",
		D3DCOMPILE_DEBUG,
		NULL,
		&buff,
		&err);

	if (success != S_OK)
	{
		std::cout << static_cast<char*>(err->GetBufferPointer()) << std::endl;
		system("pause");
		exit(-1);
	}
	else if (err != nullptr)
		err->Release();
	engine->graphics->device->CreateVertexShader(buff->GetBufferPointer(), buff->GetBufferSize(), NULL, &vtx);
	success = D3DCompileFromFile(
		L"shaders_particle.hlsl",
		NULL,
		NULL,
		"PShader",
		"ps_5_0",
		D3DCOMPILE_DEBUG,
		NULL,
		&buff,
		&err);

	if (success != S_OK)
	{
		std::cout << static_cast<char*>(err->GetBufferPointer()) << std::endl;
		system("pause");
		exit(-1);
	}
	else if (err != nullptr)
		err->Release();
	engine->graphics->device->CreatePixelShader(buff->GetBufferPointer(), buff->GetBufferSize(), NULL, &pix);

	D3D11_BLEND_DESC bDsc;
	ZeroMemory(&bDsc, sizeof(D3D11_BLEND_DESC));
	bDsc.AlphaToCoverageEnable = false;
	bDsc.IndependentBlendEnable = false;
	bDsc.RenderTarget[0].BlendEnable = true;
	bDsc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	bDsc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
	bDsc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	bDsc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
	bDsc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	bDsc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
	bDsc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	engine->graphics->device->CreateBlendState(&bDsc, &blendState);

	D3D11_DEPTH_STENCIL_DESC dsD;
	ZeroMemory(&dsD, sizeof(D3D11_DEPTH_STENCIL_DESC));

	dsD.DepthEnable = true;
	dsD.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	dsD.DepthFunc = D3D11_COMPARISON_LESS;

	engine->graphics->device->CreateDepthStencilState(&dsD, &depthState);
}

void ParticleSystem::Initialise(DirectX::XMFLOAT3 pos)
{
	position.resize(300);
	velocity.resize(300);
	lifetime = 0.0f;

	for (int i = 0; i < 300; ++i)
	{
		position[i] = pos;
		velocity[i] = { rand() / static_cast<float>(RAND_MAX) - 0.5f,
			rand() / static_cast<float>(RAND_MAX) - 0.5f,
			rand() / static_cast<float>(RAND_MAX) - 0.5f};
	}

	indices.clear();
	vertices.clear();
	vertices.resize(300 * 4);
	for (int i = 0; i < 300; ++i)
	{
		indices.push_back(0 + 4 * i);
		indices.push_back(1 + 4 * i);
		indices.push_back(2 + 4 * i);
		indices.push_back(2 + 4 * i);
		indices.push_back(1 + 4 * i);
		indices.push_back(3 + 4 * i);

		vertices[0 + 4 * i].normal = { 1.0f, 1.0f, 1.0f };
		vertices[0 + 4 * i].tex = { 0.0f, 0.0f, 0.0f };
		vertices[1 + 4 * i].normal = { 1.0f, 1.0f, 1.0f };
		vertices[1 + 4 * i].tex = { 1.0f, 0.0f, 0.0f };
		vertices[2 + 4 * i].normal = { 1.0f, 1.0f, 1.0f };
		vertices[2 + 4 * i].tex = { 0.0f, 1.0f, 0.0f };
		vertices[3 + 4 * i].normal = { 1.0f, 1.0f, 1.0f };
		vertices[3 + 4 * i].tex = { 1.0f, 1.0f, 0.0f };
	}
}

void ParticleSystem::Update()
{
	lifetime += engine->clock->DeltaT();
	if (lifetime > 0.7f)
	{
		position.clear();
		velocity.clear();
	}
	for (int i = 0; i < position.size(); ++i)
	{
		DirectX::XMStoreFloat3(&position[i], DirectX::XMVectorAdd(DirectX::XMLoadFloat3(&position[i]), DirectX::XMLoadFloat3(&velocity[i])));
	}
}

void ParticleSystem::Draw()
{
	if (!position.size()) return;

	//engine->graphics->devContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DirectX::XMVECTOR up = DirectX::XMVectorScale(engine->graphics->m_camera->obj->GetComponent<Transform>()->GetUp(), 4.5f);
	DirectX::XMVECTOR right = DirectX::XMVectorScale(engine->graphics->m_camera->obj->GetComponent<Transform>()->GetRight(), 4.5f);
	for (int i = 0; i < 300; ++i)
	{
		DirectX::XMVECTOR pos = DirectX::XMLoadFloat3(&position[i]);
		DirectX::XMStoreFloat3(&vertices[0 + 4 * i].pos, pos);
		DirectX::XMStoreFloat3(&vertices[1 + 4 * i].pos, DirectX::XMVectorAdd(pos, right));
		DirectX::XMStoreFloat3(&vertices[2 + 4 * i].pos, DirectX::XMVectorAdd(pos, up));
		DirectX::XMStoreFloat3(&vertices[3 + 4 * i].pos, DirectX::XMVectorAdd(pos, DirectX::XMVectorAdd(up, right)));
	}

	engine->graphics->devContext->VSSetShader(vtx, NULL, 0);
	engine->graphics->devContext->PSSetShader(pix, NULL, 0);

	D3D11_MAPPED_SUBRESOURCE mp;
	engine->graphics->devContext->Map(vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mp);
	memcpy(mp.pData, &(vertices)[0], vertices.size() * sizeof(Vertex));
	engine->graphics->devContext->Unmap(vertexBuffer, 0);

	engine->graphics->devContext->Map(indexBuffer, 0, D3D11_MAP_WRITE_DISCARD, NULL, &mp);
	memcpy(mp.pData, &(indices)[0], indices.size() * sizeof(unsigned));
	engine->graphics->devContext->Unmap(indexBuffer, 0);

	unsigned stride = sizeof(Vertex);
	unsigned offset = 0;
	engine->graphics->devContext->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	engine->graphics->devContext->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	engine->graphics->devContext->PSSetShaderResources(0, 1, &m_texView);

	engine->graphics->devContext->OMSetBlendState(blendState, NULL, 0xffffffff);
	engine->graphics->devContext->OMSetDepthStencilState(depthState, 1);

	engine->graphics->devContext->DrawIndexed(300 * 6, 0, 0);
}