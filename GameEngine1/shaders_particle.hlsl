Texture2DArray shaderTexture : register(t0);
SamplerState samplerWrap : register(s0);

struct VOut
{
    float4 position : SV_POSITION;
    float3 tex : TEXCOORD0;
    float3 color : TEXCOORD1;
};

cbuffer perFrame : register(b0)
{
	float4x4 worldViewMatrix;
	float4x4 projectionMatrix;
    float3 cameraWorldPos;
    float padding;
};

VOut VShader(float4 position : POSITION, float3 color : NORMAL, float3 tex : TEXCOORD0)
{
    VOut output;

    position.w = 1.0;
    output.position = mul(mul(position, worldViewMatrix), projectionMatrix);  
    output.tex = tex;
    output.color = color;

    return output;
}

float4 PShader(float4 position : SV_POSITION, float3 tex : TEXCOORD0, float3 color : TEXCOORD1) : SV_TARGET
{
    float4 texCol = shaderTexture.Sample(samplerWrap, tex);

    return texCol * float4(color, 1.0);
}