#include <windows.h>
#include <windowsx.h>
#include <iostream>
#include <fstream>
#include <DirectXMath.h>
#include <d3d11.h>
#include "CompositeObject.h"
#include "GraphicsController.h"
#include "Renderer.h"
#include "Geometry.h"
#include "InputManager.h"
#include "Clock.h"
#include "MeshLoader.h"
#include "Transform.h"
#include "ShipController.h"
#include "TrackLayout.h"
#include "Engine.h"
#include "World.h"
#include "ElementFactory.h"
#include "ParticleSystem.h"
#include "ResourceTable.h"
#include "ResourceFactory.h"
#include "SceneLoader.h"
#include "StaticBatcher.h"
#include "PropertyDict.h"
#include "Component.h"

int width = 1000;
int height = 1000;
GameEngine::Engine game;

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//Handle input messages
	if (Input::HandleMessage(hWnd, message, wParam, lParam))
		return message;
	if (game.HandleMessage(hWnd, message, wParam, lParam))
		return message;

	switch(message)
	{
	//Quit the window
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	//Otherwise use the default handling
	default:
		DefWindowProc(hWnd, message, wParam, lParam);
	}
}

void DestroyBackwards(GameEngine::Elements::CompositeObject* co)
{
	GameEngine::Elements::PropertyDict* pd = co->GetComponent<GameEngine::Elements::PropertyDict>();
	if (pd != nullptr
		&& pd->HasProperty("WOforwardTrackDirection")
		&& pd->GetProperty("WOforwardTrackDirection") == "false")
	{
		co->Destroy();
		return;
	}

	for (auto child : co->GetComponent<GameEngine::Elements::Transform>()->GetChildren())
		DestroyBackwards(child->obj.Get());
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	//Random seed
	srand(time(NULL));

	//Allocate a console and bind cout/cerr to it
	AllocConsole();
	std::ofstream consoleStream("CONOUT$", std::ios::out);
	std::cout.rdbuf(consoleStream.rdbuf());
	std::cerr.rdbuf(consoleStream.rdbuf());
	std::wofstream wConsoleStream("CONOUT$", std::ios::out);
	std::wcout.rdbuf(wConsoleStream.rdbuf());

	//Create the window class
	WNDCLASSEX winClass;
	ZeroMemory(&winClass, sizeof(WNDCLASSEX));

	winClass.cbSize = sizeof(winClass);
	winClass.style = CS_HREDRAW | CS_VREDRAW;
	winClass.lpfnWndProc = WndProc;
	winClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	winClass.hInstance = hInstance;
	winClass.lpszClassName = L"CLASS1";

	//Register the window class
	RegisterClassEx(&winClass);
	
	//Create a window with CLASS1
	HWND hWnd = CreateWindow(
		L"CLASS1",
		L"GameEngine1",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		width,
		height,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	//Finally show the windows
	ShowWindow(hWnd, nCmdShow);

	//Try to lock cursor?
	RECT cursorPos = { 400, 400, 500, 500 };
	ShowCursor(false);

	//-----------------
	// Game Initialisation
	//-----------------
	
	game.clock = new GameEngine::Time::Clock;
	game.graphics = new GameEngine::Graphics::GraphicsController(width, height, false, hWnd);
	game.world = new GameEngine::Elements::World;
	game.resources = new GameEngine::Resources::ResourceTable;

	Input::InputManager input;

	D3D11_INPUT_ELEMENT_DESC iLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, tex), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	Material* mat = game.resourceFactory->Create<Material>("Default Phong");
	mat->passes.push_back(RenderPass());
	mat->passes[0].engine = &game;

	mat->passes[0].LoadVS(L"shaders.hlsl", "VShader", iLayout, 3);
	mat->passes[0].LoadPS(L"shaders.hlsl", "PShaderTex");

	mat = game.resourceFactory->Create<Material>("Default Glow");
	mat->passes.push_back(RenderPass());
	mat->passes[0].engine = &game;

	mat->passes[0].LoadVS(L"shaders_glow.hlsl", "VShader", iLayout, 3);
	mat->passes[0].LoadPS(L"shaders_glow.hlsl", "PShaderTex");

	//HACK: Need to refactor
	ID3D10Blob *buff = nullptr, *err = nullptr;
	HRESULT success = D3DCompileFromFile(
		L"depth.hlsl",
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
	game.graphics->device->CreateVertexShader(buff->GetBufferPointer(), buff->GetBufferSize(), NULL, &game.graphics->dpthVtx);
	game.graphics->device->CreateInputLayout(
		iLayout, 3, buff->GetBufferPointer(), buff->GetBufferSize(), &game.graphics->dpthILayout);
	success = D3DCompileFromFile(
		L"depth.hlsl",
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
	game.graphics->device->CreatePixelShader(buff->GetBufferPointer(), buff->GetBufferSize(), NULL, &game.graphics->dpthPx);

	success = D3DCompileFromFile(
		L"blur.hlsl",
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
	game.graphics->device->CreateVertexShader(buff->GetBufferPointer(), buff->GetBufferSize(), NULL, &game.graphics->bloomVtx);
	success = D3DCompileFromFile(
		L"blur.hlsl",
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
	game.graphics->device->CreatePixelShader(buff->GetBufferPointer(), buff->GetBufferSize(), NULL, &game.graphics->bloomPx);

	success = D3DCompileFromFile(
		L"blur.hlsl",
		NULL,
		NULL,
		"PShaderV",
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
	game.graphics->device->CreatePixelShader(buff->GetBufferPointer(), buff->GetBufferSize(), NULL, &game.graphics->bloomPxV);
	
	//MeshData* skybox = new MeshData;
	//std::vector<std::string> tex_skybox;
	//MeshLoader::ApplyFBXWithTextures(skybox, fbxNode, "skycube1_nolight", tex_skybox);

	TrackLayout tl;
	GameEngine::Resources::Mesh* track_layout = new GameEngine::Resources::Mesh;
	//fbxNode = MeshLoader::LoadFBX("Track_Outline.fbx");

	FbxManager* fbx = FbxManager::Create();
	FbxIOSettings* ios = FbxIOSettings::Create(fbx, IOSROOT);
	fbx->SetIOSettings(ios);
	FbxImporter* fbxImporter = FbxImporter::Create(fbx, "");
	fbxImporter->Initialize("Track.fbx", -1, fbx->GetIOSettings());
	FbxScene* fbxScene = FbxScene::Create(fbx, "Track.fbx");
	fbxImporter->Import(fbxScene);
	fbxImporter->Destroy();

	//Mesh* mesh = LoadFBX(fbxScene->GetRootNode(), path);
	GameEngine::Resources::MeshLoader::ApplyFbxRecursive(track_layout, fbxScene->GetRootNode(), true);
	fbx->Destroy();
	TrackLayout::SetTrack(track_layout);

	GameEngine::Elements::CompositeObject* co = game.elementFactory->Create<GameEngine::Elements::CompositeObject>();

	GameEngine::Elements::Transform* t = co->GetComponent<GameEngine::Elements::Transform>();
	t->SetPosition({ 0.0f, 0.0f, 0.0f });
	t->SetScale({ 1.0f, 1.0f, 1.0f });
	t->m_static = true;

	GameEngine::Elements::CompositeObject* scn = GameEngine::Resources::SceneLoader::LoadFBX(&game, "Track.fbx");
	DestroyBackwards(scn);

	for (GameEngine::Renderer* r : game.graphics->rq)
	{
		std::string id = (r->m_textures.Get())[0].GetIdentifier();
		if (id.find("GLOW") == -1) continue;
		r->mat = game.resourceFactory->Create<Material>("Default Glow");
	}

	StaticBatcher::BatchFrom(scn, &game);
	StaticBatcher::CullHeirarchy(scn);

	GameEngine::Elements::CompositeObject* sky = game.elementFactory->Create<GameEngine::Elements::CompositeObject>();
	GameEngine::Renderer* r = sky->AttachComponent<GameEngine::Renderer>();
	t = sky->GetComponent<GameEngine::Elements::Transform>();
	t->SetPosition({ 0.0f, 0.0f, 0.0f });
	t->SetScale({ 1.0f, 1.0f, 1.0f });

	Material* mat1 = game.resourceFactory->Create<Material>("");
	mat1->passes.push_back(RenderPass());
	mat1->passes[0].engine = &game;

	mat1->passes[0].LoadVS(L"shaders_skybox.hlsl", "VShader", iLayout, 3);
	mat1->passes[0].LoadPS(L"shaders_skybox.hlsl", "PShaderTex");

	//mat1->LoadTGAArray(tex_skybox);

	//r->Init(mat1, skybox);

	//r = co->AttachComponent<Renderer>();
	//r->Init(mat, mesh);


	GameEngine::Elements::CompositeObject* ship = game.elementFactory->Create<GameEngine::Elements::CompositeObject>();
	t = ship->GetComponent<GameEngine::Elements::Transform>();
	t->SetPosition({ -707.0f, 13.0f, -78.0f });
	t->SetRotation(DirectX::XMQuaternionIdentity());
	t->SetScale({ 1.0f, 1.0f, 1.0f });

	ship->AttachComponent<ShipController>();

	game.particleSystem = new GameEngine::Graphics::ParticleSystem(&game);

	MSG message;
	while (true)
	{
		if (PeekMessage(&message, hWnd, NULL, NULL, true))
		{
			//Translate then handle in WndProc()
			TranslateMessage(&message);
			DispatchMessage(&message);

			if (message.message == WM_QUIT)
				break;
		}
		else
			game.Loop();
	}

	//Clean up the console
	FreeConsole();

	return 0;
}
