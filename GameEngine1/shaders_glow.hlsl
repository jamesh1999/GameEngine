Texture2DArray shaderTexture : register(t0);
SamplerState samplerWrap : register(s0);

Texture2D shadowMap : register(t1);
SamplerState samplerClamp : register(s1);

struct VOut
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 viewDir : TEXCOORD1;
    float3 tex : TEXCOORD0;
    float3 color : TEXCOORD2;
    float4 lightViewPos : TEXCOORD3;
    float3 lightPos : TEXCOORD4;
};

cbuffer perFrame : register(b0)
{
	float4x4 worldViewMatrix;
	float4x4 projectionMatrix;
    float3 cameraWorldPos;
    float padding;
};

cbuffer light : register(b2)
{
	float4x4 lightViewMatrix;
	float4x4 lightProjMatrix;
	float3 lightPosition;
	float buffer;
};

cbuffer perObject : register(b1)
{
    float4x4 objWorldTransform;
};

VOut VShader(float4 position : POSITION, float3 color : NORMAL, float3 tex : TEXCOORD0)
{
    VOut output;

    position.w = 1.0;
    float4 worldPos = mul(position, objWorldTransform);
    output.position = mul(mul(worldPos, worldViewMatrix), projectionMatrix);
    output.normal = normalize(mul(float4(color, 0.0), objWorldTransform)).xyz;   
    output.tex = tex;
    output.viewDir = normalize(cameraWorldPos - mul(position, objWorldTransform).xyz);
    output.color = float3(1.0, 1.0, 1.0);

    output.lightViewPos = mul(mul(worldPos, lightViewMatrix), lightProjMatrix);
    output.lightPos = lightPosition.xyz - worldPos.xyz;

    return output;
}

float4 PShaderTex(float4 position : SV_POSITION, float3 norm : NORMAL, float3 view : TEXCOORD1, float3 tex : TEXCOORD0, float3 color : TEXCOORD2, float4 lightViewPos : TEXCOORD3, float3 lightPos : TEXCOORD4) : SV_TARGET
{
    return shaderTexture.Sample(samplerWrap, tex);
}