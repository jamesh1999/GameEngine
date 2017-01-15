#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <tuple>

struct TGAHeader
{
	char d1[12];
	short width;
	short height;
	char bpp;
	char d2;
};

class RenderPass
{
public:
	ID3D11VertexShader* vs = nullptr;
	ID3D11PixelShader* ps = nullptr;
	ID3D11InputLayout* layout = nullptr;

	void LoadVS(std::wstring filename, std::string entry, D3D11_INPUT_ELEMENT_DESC* inputDesc, int numElements);
	void LoadPS(std::wstring filename, std::string entry);
};

class Material
{
	typedef struct
	{
		uint8_t* data;
		int w;
		int h;
	} TexData;

	ID3D11Texture2D* m_tex = nullptr;

	void setTextureData(uint8_t*, int, int);
	void setTextureData(std::vector<TexData>);
	std::tuple<uint8_t*, int, int> getTGAData(std::string);

public:
	std::vector<RenderPass> passes;

	ID3D11ShaderResourceView* m_texView = nullptr;
	ID3D11SamplerState* m_samplerState = nullptr;

	Material() = default;
	~Material();

	Material(const Material&);
	Material(Material&&) noexcept;
	Material& operator=(const Material&);
	Material& operator=(Material&&) noexcept;

	ID3D11ShaderResourceView* GetTexture() const;
	ID3D11SamplerState* GetSampler() const;
	void LoadTGA(std::string);
	void LoadTGAArray(std::vector<std::string>);
	void SetTexture(DirectX::XMFLOAT3*, int, int);
};

class MaterialGroup
{
private:
	std::vector<Material*> materials;

public:
	MaterialGroup() = default;
	~MaterialGroup();

	MaterialGroup(const MaterialGroup&);
	MaterialGroup(MaterialGroup&&) noexcept;
	MaterialGroup& operator=(const MaterialGroup&);
	MaterialGroup& operator=(MaterialGroup&&) noexcept;

	Material* operator[](int);
	int Size() const;

	void AddMaterial(Material*);
};

#endif
