cbuffer cbPerObject
{
    float4x4 WVP;
};

struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;

    float2 Tex : TEXCOORD;
};

Texture2D shaderTexture : TEXTURE :register(t0);
SamplerState SampleType : SAMPLER : register(s0);

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTex:TEXCOORD)
{
    VS_OUTPUT output;

    output.Pos = mul(inPos, WVP);
    output.Tex = inTex;
    return output;
}


float4 PS(VS_OUTPUT input) : SV_TARGET
{ 
    float4 textureColor;
    textureColor = shaderTexture.Sample(SampleType, input.Tex);
    return textureColor; //input.Color;
}