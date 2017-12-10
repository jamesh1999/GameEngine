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
#include "ObjectManager.h"
#include "TrackLayout.h"
#include "Engine.h"
#include "World.h"
#include "ElementFactory.h"

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
	game.world = new GameEngine::ObjectSystem::World();
	game.elementFactory = new GameEngine::ObjectSystem::ElementFactory();
	game.elementFactory->engine = &game;

	Input::InputManager input;
	MeshLoader meshLoader;

	D3D11_INPUT_ELEMENT_DESC iLayout[]
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, pos), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(Vertex, tex), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

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

	MeshData* mesh = new MeshData;
	FbxNode* fbxNode = MeshLoader::LoadFBX("Track.fbx");
	MeshLoader::PrintFBXHeirachy(fbxNode);
	std::vector<std::string> tex;
	MeshLoader::ApplyFBXWithTextures(mesh, fbxNode, "", tex);
	
	MeshData* skybox = new MeshData;
	std::vector<std::string> tex_skybox;
	MeshLoader::ApplyFBXWithTextures(skybox, fbxNode, "skycube1_nolight", tex_skybox);

	TrackLayout tl;
	MeshData* track_layout = new MeshData;
	//fbxNode = MeshLoader::LoadFBX("Track_Outline.fbx");
	MeshLoader::ApplyFBX(track_layout, fbxNode, "", true);
	TrackLayout::SetTrack(&(*track_layout)[0]);

	ObjectManager om(&game);

	CompositeObject* co = game.elementFactory->Create<CompositeObject>();

	Transform* t = co->GetComponent<Transform>();
	t->SetPosition({ 0.0f, 0.0f, 0.0f });
	t->SetScale({ 1.0f, 1.0f, 1.0f });
	t->m_static = true;

	Material* mat = new Material(&game);
	mat->passes.push_back(RenderPass());
	mat->passes[0].engine = &game;

	mat->passes[0].LoadVS(L"shaders.hlsl", "VShader", iLayout, 3);
	mat->passes[0].LoadPS(L"shaders.hlsl", "PShaderTex");

	mat->LoadTGAArray(tex);

	MaterialGroup mg;
	mg.AddMaterial(mat);

	Renderer* r = co->AttachComponent<Renderer>();
	r->Init(mg, mesh);

	

	
	CompositeObject* sky = game.elementFactory->Create<CompositeObject>();
	r = sky->AttachComponent<Renderer>();
	t = sky->GetComponent<Transform>();
	t->SetPosition({ 0.0f, 0.0f, 0.0f });
	t->SetScale({ 1.0f, 1.0f, 1.0f });

	Material* mat1 = new Material(&game);
	mat1->passes.push_back(RenderPass());
	mat1->passes[0].engine = &game;

	mat1->passes[0].LoadVS(L"shaders_skybox.hlsl", "VShader", iLayout, 3);
	mat1->passes[0].LoadPS(L"shaders_skybox.hlsl", "PShaderTex");

	mat1->LoadTGAArray(tex_skybox);

	MaterialGroup mg1;
	mg1.AddMaterial(mat1);

	//r->Init(mg1, skybox);


	CompositeObject* ship = game.elementFactory->Create<CompositeObject>();
	t = ship->GetComponent<Transform>();
	t->SetPosition({ -707.0f, 13.0f, -78.0f });
	t->SetRotation(DirectX::XMQuaternionIdentity());
	t->SetScale({ 1.0f, 1.0f, 1.0f });

	ship->AttachComponent<ShipController>();
	

	CompositeObject* root = game.elementFactory->Create<CompositeObject>();

	CompositeObject* light = game.elementFactory->Create<CompositeObject>();
	t = light->GetComponent<Transform>();
	//t->SetPosition({ 0.0f, 120.0f, 0.0f });
	//t->SetRotation(DirectX::XMQuaternionRotationAxis({ 1.0f, 0.0f, 0.0f }, DirectX::XM_PIDIV2));
	//t->SetScale({ 1.0f, 1.0f, 1.0f });
	t->parent = ship->GetComponent<Transform>();
	t->SetRotation(DirectX::XMQuaternionIdentity());
	t->SetPosition({0.0f, 0.0f, 10.0f});
	t->SetScale({ 1.0f, 1.0f, 1.0f });

	//t = root.GetComponent<Transform>();
	//t->SetPosition({ -700.0f, 0.0f, 190.0f });
	//t->SetRotation(DirectX::XMQuaternionIdentity());
	//t->SetScale({ 1.0f, 1.0f, 1.0f });
	light->AttachComponent<Light>();
	game.graphics->SetLight(light->GetComponent<Light>());

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
