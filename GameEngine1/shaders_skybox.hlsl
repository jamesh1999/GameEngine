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

    position.w = 0.0;
    float4 worldPos = mul(position, objWorldTransform);
    float4 position1 = mul(worldPos, worldViewMatrix);
    position1.w = 1.0;
    output.position = mul(position1, projectionMatrix);
    output.normal = normalize(mul(float4(color, 0.0), objWorldTransform));   
    output.tex = tex;
    output.viewDir = normalize(cameraWorldPos - mul(position, objWorldTransform).xyz);
    output.color = float3(1.0, 1.0, 1.0);

    output.lightViewPos = mul(mul(worldPos, lightViewMatrix), lightProjMatrix);
    output.lightPos = lightPosition.xyz - worldPos.xyz;

    return output;
}

float4 PShader(float4 position : SV_POSITION, float3 norm : NORMAL, float3 view : TEXCOORD1, float3 tex : TEXCOORD0, float3 color : TEXCOORD2, float4 lightViewPos : TEXCOORD3, float3 lightPos : TEXCOORD4) : SV_TARGET
{
    float4 c = float4(0.2, 0.2, 0.2, 1.0);
    float4 specular = float4(0.0,0.0,0.0,0.0);

    float2 lightTexCoord;
    lightTexCoord.x = lightViewPos.x / lightViewPos.w / 2.0 + 0.5;
    lightTexCoord.y = -lightViewPos.y / lightViewPos.w / 2.0 + 0.5;

    //Check pixel in light view
    if( (saturate(lightTexCoord.x) == lightTexCoord.x)
     && (saturate(lightTexCoord.y) == lightTexCoord.y))
    {
    	float lightDepth = shadowMap.Sample(samplerClamp, lightTexCoord).r;
    	float actualDepth = lightViewPos.z / lightViewPos.w - 0.001;

    	if(actualDepth < lightDepth)
    	{

    		float3 lightDir = normalize(float3(0.0, 1.0, 1.0));
    		float intensity = dot(-lightDir, norm);

		    if(intensity > 0.0)
		    {
		        c += intensity * float4(0.6, 0.6, 0.6, 1.0);

		        float3 reflection = normalize(lightDir + 2 * intensity * norm); 
		        specular = pow(saturate(dot(reflection, view)), 10.0) * 1.2;
		    }
		}
	}

	//float4 texCol = shaderTexture.Sample(samplerWrap, tex);

	return float4(color, 1.0) * c + specular;
}

float4 PShaderTex(float4 position : SV_POSITION, float3 norm : NORMAL, float3 view : TEXCOORD1, float3 tex : TEXCOORD0, float3 color : TEXCOORD2, float4 lightViewPos : TEXCOORD3, float3 lightPos : TEXCOORD4) : SV_TARGET
{
    float4 c = float4(0.2, 0.2, 0.2, 1.0);
    float4 specular = float4(0.0,0.0,0.0,0.0);

    float2 lightTexCoord;
    lightTexCoord.x = lightViewPos.x / lightViewPos.w / 2.0 + 0.5;
    lightTexCoord.y = -lightViewPos.y / lightViewPos.w / 2.0 + 0.5;

    //Check pixel in light view
    if( (saturate(lightTexCoord.x) == lightTexCoord.x)
     && (saturate(lightTexCoord.y) == lightTexCoord.y))
    {
    	float lightDepth = shadowMap.Sample(samplerClamp, lightTexCoord).r;
    	float actualDepth = lightViewPos.z / lightViewPos.w - 0.0005;

    	if(actualDepth < lightDepth)
    	{

    		float3 lightDir = normalize(lightPos);
    		float intensity = dot(lightDir, norm);
    		float multiplier = rsqrt(length(lightPos) / 60.0);

		    if(intensity > 0.0)
		    {
		        c += intensity * float4(0.8, 0.8, 0.8, 1.0) * multiplier;

		        float3 reflection = normalize(-lightDir + 2 * intensity * norm); 
		        specular = pow(saturate(dot(reflection, view)), 10.0) * 1.2 * multiplier;
		    }
		}
	}

    float4 texCol = shaderTexture.Sample(samplerWrap, tex);

    return texCol;
}