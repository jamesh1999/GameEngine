#include "Component.h"
#include "ShipController.h"
#include "InputManager.h"
#include "Transform.h"
#include "CompositeObject.h"
#include "Clock.h"
#include "GraphicsController.h"
#include "TrackLayout.h"
#include <iostream>
#include <tuple>
#include <cmath>
#include "MeshLoader.h"
#include "CompositeObject.h"
#include "ElementFactory.h"
#include "ParticleSystem.h"
#include "ResourceFactory.h"

void ShipController::UpdateBase()
{
	GameEngine::Elements::Transform* transform = obj->GetComponent<GameEngine::Elements::Transform>();
	DirectX::XMVECTOR pos = transform->GetPosition();

	DirectX::XMVECTOR norm, basePos;
	std::tie(norm, basePos, overTri) = TrackLayout::GetNormal(pos);

	base->GetComponent<GameEngine::Elements::Transform>()->SetPosition(basePos);
	DirectX::XMStoreFloat3(&curNorm, norm);
}

void ShipController::Update()
{
	UpdateBase();

	DirectX::XMVECTOR facingDir = DirectX::XMLoadFloat3(&facingDirection);
	DirectX::XMVECTOR norm = DirectX::XMLoadFloat3(&curNorm);

	//Project facing direction onto plane of new track segment
	facingDir = DirectX::XMVector3Normalize(
		DirectX::XMVectorSubtract(
			facingDir,
			DirectX::XMVectorMultiply(
				DirectX::XMVector3Dot(
					norm,
					facingDir),
				norm)));

	GameEngine::Elements::Transform* transform = obj->GetComponent<GameEngine::Elements::Transform>();

	DirectX::XMVECTOR shipPos = transform->GetPosition();
	float height = 0.0f;
	float dist = engine->clock->DeltaT() * 40.0;
	float dpos = 0.0f;
	if (Input::InputManager::KeyIsPressed(Input::KeyW))
	{
		dpos += dist;

		if (Input::InputManager::KeyIsPressed((Input::KeyF)))
			dpos *= 10;
	}
	if (Input::InputManager::KeyIsPressed(Input::KeyS))
	{
		dpos -= dist * 0.5f;
	}
	if (Input::InputManager::KeyIsPressed(Input::KeyQ))
	{
		height += dist * 0.4f;
	}
	if (Input::InputManager::KeyIsPressed(Input::KeyE))
	{
		height -= dist * 0.4f;
	}

	float drot = 0.0f;
	if (Input::InputManager::KeyIsPressed(Input::KeyD))
	{
		drot += dist * 0.03;
		curRoll -= dist * rollSpeed;
		if (curRoll < -rollMax)
			curRoll = -rollMax;
	}
	else if (Input::InputManager::KeyIsPressed(Input::KeyA))
	{
		drot -= dist * 0.03;
		curRoll += dist * rollSpeed;
		if (curRoll > rollMax)
			curRoll = rollMax;
	}
	else if(curRoll < 0.0f)
	{
		curRoll += dist * rollSpeed;
		if (curRoll > 0.0f)
			curRoll = 0.0f;
	}
	else if (curRoll > 0.0f)
	{
		curRoll -= dist * rollSpeed;
		if (curRoll < 0.0f)
			curRoll = 0.0f;
	}

	//Apply new velocity vector
	DirectX::XMVECTOR v = 
		DirectX::XMVectorAdd(
			DirectX::XMLoadFloat3(&velocity),
			DirectX::XMVectorScale(transform->GetForwards(), dpos * acceleration)
		);

	//Calculate track repulsion force
	DirectX::XMFLOAT3 basepos;
	DirectX::XMStoreFloat3(&basepos, base->GetComponent<GameEngine::Elements::Transform>()->GetPosition());

	if (overTri)
	{
		float vDist = DirectX::XMVectorGetX(
			DirectX::XMVector3Dot(
				DirectX::XMVectorSubtract(
					shipPos,
					base->GetComponent<GameEngine::Elements::Transform>()->GetPosition()),
				norm
			)) - 5.0f;
		
		if (vDist > engine->clock->DeltaT() * vertSpeed)
			vDist = engine->clock->DeltaT() * vertSpeed;
		else if (vDist < -engine->clock->DeltaT() * vertSpeed)
			vDist = -engine->clock->DeltaT() * vertSpeed;

		//Add on repulsion force
		shipPos = DirectX::XMVectorAdd(DirectX::XMVectorScale(norm, -vDist), shipPos);
	}

	shipPos = DirectX::XMVectorAdd(
		shipPos,
		DirectX::XMVectorScale(v, engine->clock->DeltaT())
	);
	transform->SetPosition(shipPos);

	//Add drag
	DirectX::XMVECTOR dragV =
		DirectX::XMVectorScale(
			DirectX::XMVectorPow(
				v,
				{ 2.0f, 2.0f, 2.0f }),
			drag * engine->clock->DeltaT()
		);

	DirectX::XMFLOAT3 manualD, manualV;
	DirectX::XMStoreFloat3(&manualD, dragV);
	DirectX::XMStoreFloat3(&manualV, v);

	if(manualV.x < 0.0f)
	{
		manualV.x += std::abs(manualD.x);
		if (manualV.x > 0.0f)
			manualV.x = 0.0f;
	}
	else if(manualV.x > 0.0f)
	{
		manualV.x -= std::abs(manualD.x);
		if (manualV.x < 0.0f)
			manualV.x = 0.0f;
	}
	if (manualV.y < 0.0f)
	{
		manualV.y += std::abs(manualD.y);
		if (manualV.y > 0.0f)
			manualV.y = 0.0f;
	}
	else if (manualV.y > 0.0f)
	{
		manualV.y -= std::abs(manualD.y);
		if (manualV.y < 0.0f)
			manualV.y = 0.0f;
	}
	if (manualV.z < 0.0f)
	{
		manualV.z += std::abs(manualD.z);
		if (manualV.z > 0.0f)
			manualV.z = 0.0f;
	}
	else if (manualV.z > 0.0f)
	{
		manualV.z -= std::abs(manualD.z);
		if (manualV.z < 0.0f)
			manualV.z = 0.0f;
	}

	v = DirectX::XMLoadFloat3(&manualV);
	DirectX::XMStoreFloat3(&velocity, v);

	//Rotate around normal
	facingDir = DirectX::XMVector3TransformNormal(
		facingDir,
		DirectX::XMMatrixRotationQuaternion(
			DirectX::XMQuaternionRotationAxis(
				norm,
				drot)));

	DirectX::XMStoreFloat3(&facingDirection, facingDir);

	//Get current normal
	DirectX::XMVECTOR currentNormal = norm;//transform->GetUp();

	//Interpolate to find desired normal for this frame
	/*float angle = DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenNormals(norm, currentNormal));
	float interpolate = alignSpeed * Time::TimeManager::DeltaT() / angle;
	if (interpolate > 1.0f || interpolate < 0.0f || isnan(interpolate))
		interpolate = 1.0f;
	currentNormal = DirectX::XMVectorLerp(currentNormal, norm, interpolate);*/

	//Project facing direction onto plane of current normal
	facingDir = DirectX::XMVector3Normalize(
		DirectX::XMVectorSubtract(
			facingDir,
			DirectX::XMVectorMultiply(
				DirectX::XMVector3Dot(
					currentNormal,
					facingDir),
				currentNormal)));

	//Compute quaternion to map {0,1,0} to current normal
	DirectX::XMVECTOR qa = DirectX::XMVector3Normalize(DirectX::XMVector3Cross({ 0.0f, 1.0f, 0.0f }, currentNormal));
	DirectX::XMVECTOR dot = DirectX::XMVector3Dot(
		DirectX::XMVector3Normalize(currentNormal),
		{ 0.0f, 1.0f, 0.0f }
	);
	float x = DirectX::XMVectorGetX(dot);
	if (x > 1.0f)
		x = 1.0f;
	else if (x < -1.0f)
		x = -1.0f;
	float a = std::acos(x) / 2;
	qa = DirectX::XMVectorMultiply(
		DirectX::XMVectorSetW(qa, 1.0f),
		{ std::sin(a), std::sin(a), std::sin(a), std::cos(a) }
	);

	//Compute quaternion to map {0,0,1} to forwards direction
	DirectX::XMVECTOR va = DirectX::XMVector3Rotate(
		{ 0.0f, 0.0f, 1.0f },
		qa
	);
	DirectX::XMVECTOR qb = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(va, facingDir));
	dot = DirectX::XMVector3Dot(
		DirectX::XMVector3Normalize(facingDir),
		va
	);
	x = DirectX::XMVectorGetX(dot);
	if (x > 1.0f)
		x = 1.0f;
	else if (x < -1.0f)
		x = -1.0f;
	a = std::acos(x) / 2;
	qb = DirectX::XMVectorMultiply(
		DirectX::XMVectorSetW(qb, 1.0f),
		{ std::sin(a), std::sin(a), std::sin(a), std::cos(a) }
	);

	DirectX::XMVECTOR rot = DirectX::XMQuaternionMultiply(qa, qb);
	transform->SetRotation(rot);
	
	DirectX::XMVECTOR camPos =
		DirectX::XMVector3TransformCoord(
	{ 0.0f, 3.0f, -20.0f },
			transform->GetTransform());
	cam->GetComponent<GameEngine::Elements::Transform>()->SetPosition(camPos);
	cam->GetComponent<GameEngine::Elements::Transform>()->SetRotation(transform->GetRotation());

	model->GetComponent<GameEngine::Elements::Transform>()->SetRotation(DirectX::XMQuaternionRotationAxis({0.0f, 0.0f, 1.0f}, curRoll));

	//shipRot = DirectX::XMVectorSetByIndex(shipRot, roll, 2);
	//transform->SetRotation(shipRot);

	//if (DirectX::XMVectorGetY(shipPos) > 8.0f && std::fabs(DirectX::XMVectorGetX(v)) > 0.001)
		//engine->graphics->RemoveRenderer(obj->GetComponent<GameEngine::Renderer>());

	if (Input::InputManager::KeyIsPressed(Input::KeyZ))
	{
		DirectX::XMFLOAT3 wp;
		DirectX::XMStoreFloat3(&wp, transform->GetPosition());
		engine->particleSystem->Initialise(wp);
	}
}

void ShipController::Create()
{
	cam = engine->elementFactory->Create<GameEngine::Elements::CompositeObject>();
	model = engine->elementFactory->Create<GameEngine::Elements::CompositeObject>();
	base = engine->elementFactory->Create<GameEngine::Elements::CompositeObject>();
	light = engine->elementFactory->Create<GameEngine::Elements::CompositeObject>();

	//Init camera
	cam->AttachComponent<Camera>();
	cam->GetComponent<GameEngine::Elements::Transform>()->SetScale({ 1.0f,1.0f,1.0f });

	//Init model child object
	GameEngine::Resources::Mesh* mesh = engine->resourceFactory->Create<GameEngine::Resources::Mesh>("test.fbx;lodGroup1/ship/");

	GameEngine::Elements::Transform* t = model->GetComponent<GameEngine::Elements::Transform>();
	t->SetPosition({ 0.0f, 0.0f, 0.0f });
	t->SetRotation(DirectX::XMQuaternionIdentity());
	t->SetScale({ 1.0f, 1.0f, 1.0f });
	t->SetParent(obj->GetComponent<GameEngine::Elements::Transform>());

	D3D11_INPUT_ELEMENT_DESC iLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, tex), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	Material* mat = engine->resourceFactory->Create<Material>("");
	mat->passes.push_back(RenderPass());
	mat->passes[0].engine = engine;
	mat->passes[0].LoadVS(L"shaders.hlsl", "VShader", iLayout, 3);
	mat->passes[0].LoadPS(L"shaders.hlsl", "PShaderTex");

	GameEngine::Renderer* r = model->AttachComponent<GameEngine::Renderer>();
	r->Init(mat, mesh);
	GameEngine::Resources::Texture* tex = engine->resourceFactory->Create<GameEngine::Resources::Texture>("test.tga");
	r->SetTexture(tex);

	//Init light
	t = light->GetComponent<GameEngine::Elements::Transform>();
	t->SetRotation(DirectX::XMQuaternionIdentity());
	t->SetPosition({ 0.0f, 0.0f, 10.0f });
	t->SetScale({ 1.0f, 1.0f, 1.0f });
	t->SetParent(obj->GetComponent<GameEngine::Elements::Transform>());

	light->AttachComponent<Light>();
	engine->graphics->SetLight(light->GetComponent<Light>());

	facingDirection = { 0.0f, 0.0f, 1.0f };
	velocity = { 0.0f, 0.0f, 0.0f };
}

ShipController::ShipController()
{
}

ShipController::~ShipController()
{
}
