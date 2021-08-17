cbuffer cbPerObject
{
	float4x4 WVP;
};
struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
	float2 Tex : TEXCOORD;
};

VS_OUTPUT VS(float4 inPos : POSITION, float2 inTex : TEXCOORD)
{
	VS_OUTPUT output;

	output.Pos = mul(inPos, WVP);
	output.Tex = inTex;

	return output;
}
