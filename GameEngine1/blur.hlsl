Texture2D shaderTexture : register(t0);
SamplerState samplerWrap : register(s0);

struct VOut
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};

VOut VShader(float4 position : POSITION, float3 color : NORMAL, float3 tex : TEXCOORD0, uint id : SV_VertexID)
{
	VOut output;

	float x = 1.0 - float((id & 1) << 2);
    float y = -1.0 + float((id & 2) << 1);
    output.tex = float2((x+1.0)*0.5, (1.0-y)*0.5);
    output.position = float4(x, y, 0, 1);

    return output;
}

float4 PShader(float4 pos : SV_POSITION, float2 tex : TEXCOORD0) : SV_Target0
{
    float weight0, weight1, weight2, weight3, weight4, weight5, weight6;
    float normalization;
    float4 color;

    float w;
    float h;
    shaderTexture.GetDimensions(w, h);
    float offset = 1 / w;

    // Create the weights that each neighbor pixel will contribute to the blur.
    weight0 = 1.0f;
    weight1 = 0.94f;
    weight2 = 0.7f;
    weight3 = 0.42f;
    weight4 = 0.24f;
    weight5 = 0.14f;
    weight6 = 0.06f;

    // Create a normalized value to average the weights out a bit.
    normalization = (weight0 + 2.0f * (weight1 + weight2 + weight3 + weight4 + weight5 + weight6));

    // Normalize the weights.
    weight0 = weight0 / normalization;
    weight1 = weight1 / normalization;
    weight2 = weight2 / normalization;
    weight3 = weight3 / normalization;
    weight4 = weight4 / normalization;
    weight5 = weight5 / normalization;
    weight6 = weight6 / normalization;

    // Initialize the color to black.
    color = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Add the nine horizontal pixels to the color by the specific weight of each.
    color += shaderTexture.Sample(samplerWrap, tex-offset*6*float2(1.0, 0.0)) * weight6;
    color += shaderTexture.Sample(samplerWrap, tex-offset*5*float2(1.0, 0.0)) * weight5;
    color += shaderTexture.Sample(samplerWrap, tex-offset*4*float2(1.0, 0.0)) * weight4;
    color += shaderTexture.Sample(samplerWrap, tex-offset*3*float2(1.0, 0.0)) * weight3;
    color += shaderTexture.Sample(samplerWrap, tex-offset*2*float2(1.0, 0.0)) * weight2;
    color += shaderTexture.Sample(samplerWrap, tex-offset*1*float2(1.0, 0.0)) * weight1;
    color += shaderTexture.Sample(samplerWrap, tex) * weight0;
    color += shaderTexture.Sample(samplerWrap, tex+offset*6*float2(1.0, 0.0)) * weight6;
    color += shaderTexture.Sample(samplerWrap, tex+offset*5*float2(1.0, 0.0)) * weight5;
    color += shaderTexture.Sample(samplerWrap, tex+offset*4*float2(1.0, 0.0)) * weight4;
    color += shaderTexture.Sample(samplerWrap, tex+offset*3*float2(1.0, 0.0)) * weight3;
    color += shaderTexture.Sample(samplerWrap, tex+offset*2*float2(1.0, 0.0)) * weight2;
    color += shaderTexture.Sample(samplerWrap, tex+offset*1*float2(1.0, 0.0)) * weight1;

    // Set the alpha channel to one.
    color.a = 1.0;

    return color;
}

float4 PShaderV(float4 pos : SV_POSITION, float2 tex : TEXCOORD0) : SV_Target0
{
    float weight0, weight1, weight2, weight3, weight4, weight5, weight6;
    float normalization;
    float4 color;

    float w;
    float h;
    shaderTexture.GetDimensions(w, h);
    float offset = 1 / h;

    // Create the weights that each neighbor pixel will contribute to the blur.
    weight0 = 1.0f;
    weight1 = 0.94f;
    weight2 = 0.7f;
    weight3 = 0.42f;
    weight4 = 0.24f;
    weight5 = 0.14f;
    weight6 = 0.06f;

    // Create a normalized value to average the weights out a bit.
    normalization = (weight0 + 2.0f * (weight1 + weight2 + weight3 + weight4 + weight5 + weight6));

    // Normalize the weights.
    weight0 = weight0 / normalization;
    weight1 = weight1 / normalization;
    weight2 = weight2 / normalization;
    weight3 = weight3 / normalization;
    weight4 = weight4 / normalization;
    weight5 = weight5 / normalization;
    weight6 = weight6 / normalization;

    // Initialize the color to black.
    color = float4(0.0f, 0.0f, 0.0f, 0.0f);

    // Add the nine horizontal pixels to the color by the specific weight of each.
    color += shaderTexture.Sample(samplerWrap, tex-offset*6*float2(0.0, 1.0)) * weight6;
    color += shaderTexture.Sample(samplerWrap, tex-offset*5*float2(0.0, 1.0)) * weight5;
    color += shaderTexture.Sample(samplerWrap, tex-offset*4*float2(0.0, 1.0)) * weight4;
    color += shaderTexture.Sample(samplerWrap, tex-offset*3*float2(0.0, 1.0)) * weight3;
    color += shaderTexture.Sample(samplerWrap, tex-offset*2*float2(0.0, 1.0)) * weight2;
    color += shaderTexture.Sample(samplerWrap, tex-offset*1*float2(0.0, 1.0)) * weight1;
    color += shaderTexture.Sample(samplerWrap, tex) * weight0;
    color += shaderTexture.Sample(samplerWrap, tex+offset*6*float2(0.0, 1.0)) * weight6;
    color += shaderTexture.Sample(samplerWrap, tex+offset*5*float2(0.0, 1.0)) * weight5;
    color += shaderTexture.Sample(samplerWrap, tex+offset*4*float2(0.0, 1.0)) * weight4;
    color += shaderTexture.Sample(samplerWrap, tex+offset*3*float2(0.0, 1.0)) * weight3;
    color += shaderTexture.Sample(samplerWrap, tex+offset*2*float2(0.0, 1.0)) * weight2;
    color += shaderTexture.Sample(samplerWrap, tex+offset*1*float2(0.0, 1.0)) * weight1;

    // Set the alpha channel to one.
    color.a = 1.0;

    return pow(color, 1/2.2);
}