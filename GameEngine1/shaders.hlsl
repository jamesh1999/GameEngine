Texture2DArray shaderTexture : register(t0);
SamplerState samplerState : register(s0);

struct VOut
{
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float3 viewDir : TEXCOORD1;
    float3 tex : TEXCOORD0;
    float3 color : TEXCOORD2;
};

cbuffer perFrame : register(b0)
{
	float4x4 worldViewMatrix;
	float4x4 projectionMatrix;
    float3 cameraWorldPos;
    float padding;
};

cbuffer perObject : register(b1)
{
    float4x4 objWorldTransform;
};

VOut VShader(float4 position : POSITION, float3 color : NORMAL, float3 tex : TEXCOORD0)
{
    VOut output;

    position.w = 1.0;
    output.position = mul(mul(mul(position, objWorldTransform), worldViewMatrix), projectionMatrix);
    output.normal = normalize(mul(float4(color, 0.0), objWorldTransform));   
    output.tex = tex;
    output.viewDir = normalize(cameraWorldPos - mul(position, objWorldTransform).xyz);
    output.color = float3(1.0, 1.0, 1.0);

    return output;
}

float4 PShader(float4 position : SV_POSITION, float3 norm : NORMAL, float3 view : TEXCOORD1, float3 tex : TEXCOORD0, float3 color : TEXCOORD2) : SV_TARGET
{
	float3 lightDir = normalize(float3(0.0, 0.0, 1.0));
	float intensity = dot(-lightDir, norm);
	float4 c = float4(0.2, 0.2, 0.2, 1.0);
	float4 specular = float4(0.0,0.0,0.0,0.0);

	if(intensity > 0.0)
	{
		c += intensity * float4(0.6, 0.6, 0.6, 1.0);

        float3 reflection = normalize(lightDir + 2 * intensity * norm); 
        specular = pow(saturate(dot(reflection, view)), 10.0) * 1.2;
	}

	//float4 texCol = shaderTexture.Sample(samplerState, tex);

	return float4(color, 1.0) * c + specular;
}

float4 PShaderTex(float4 position : SV_POSITION, float3 norm : NORMAL, float3 view : TEXCOORD1, float3 tex : TEXCOORD0, float3 color : TEXCOORD2) : SV_TARGET
{
    float3 lightDir = normalize(float3(0.0, 1.0, 1.0));
    float intensity = dot(-lightDir, norm);
    float4 c = float4(0.2, 0.2, 0.2, 1.0);
    float4 specular = float4(0.0,0.0,0.0,0.0);

    if(intensity > 0.0)
    {
        c += intensity * float4(0.6, 0.6, 0.6, 1.0);

        float3 reflection = normalize(lightDir + 2 * intensity * norm); 
        specular = pow(saturate(dot(reflection, view)), 10.0) * 1.2;
    }

    float4 texCol = shaderTexture.Sample(samplerState, tex);

    return texCol * c + specular;
}