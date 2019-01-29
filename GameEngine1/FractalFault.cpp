#include <cstdlib>
#include <cmath>
#include <d3dcompiler.h>
#include <Windows.h>
#include "GraphicsController.h"
#include <minwinbase.h>
#include "FractalFault.h"

Generation::FractalFault* Generation::FractalFault::instance = nullptr;

Generation::FractalFault::FractalFault()
{
	/*//Create fractal fault compute shader
	ID3D10Blob* cs;
	D3DCompileFromFile(L"fractalfault.hlsl", NULL, NULL, "CS_Fault", "cs_5_0", D3DCOMPILE_DEBUG, 0, &cs, NULL);

	GraphicsController::instance->device->CreateComputeShader(cs->GetBufferPointer(), cs->GetBufferSize(), NULL, &m_shader);

	//Create structured buffer to pass fault data to GPU
	D3D11_BUFFER_DESC sbDesc;
	ZeroMemory(&sbDesc, sizeof(D3D11_BUFFER_DESC));
	sbDesc.Usage = D3D11_USAGE_DYNAMIC;
	sbDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	sbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	sbDesc.StructureByteStride = sizeof(Fault);
	
	ID3D11Buffer* sb;
	GraphicsController::instance->device->CreateBuffer(&sbDesc, NULL, &sb);
	GraphicsController::instance->device->CreateUnorderedAccessView(sb, NULL, &m_faults);
	sb->Release();

	//Create texture to read back GPU data
	D3D11_TEXTURE2D_DESC txDesc;
	ZeroMemory(&txDesc, sizeof(D3D11_TEXTURE2D_DESC));
	txDesc.Format = DXGI_FORMAT_R32_FLOAT;
	txDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	txDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	txDesc.Height = 512;
	txDesc.Width = 512;
	txDesc.Usage = D3D11_USAGE_STAGING;

	ID3D11Texture2D* tex;
	GraphicsController::instance->device->CreateTexture2D(&txDesc, NULL, &tex);
	GraphicsController::instance->device->CreateUnorderedAccessView(tex, NULL, &m_RWTex);
	tex->Release();*/
}

Generation::FractalFault::~FractalFault()
{
	//m_shader->Release();
	//m_faults->Release();
	//m_RWTex->Release();
}

void Generation::FractalFault::Generate(float* out, int w, int h, int n)
{
	//for (int i = 0; i < w * h; ++i)
	//out[i] = 0.0f;

	for (int i = 0; i < n; ++i)
	{
		float grad = tan((rand() - RAND_MAX / 2) / 3.1416f);
		int offset = rand() % h;
		bool greater = rand() % 2 == 0;
		float val = 1.0f / (120 + i);

		for (int y = 0; y < h; ++y)
			for (int x = 0; x < w; ++x)
			{
				if (greater == (y > grad * x + offset))
				{
					out[y * w + x] += val;
				}
				else
				{
					out[y * w + x] -= val;
				}
			}
	}
}
