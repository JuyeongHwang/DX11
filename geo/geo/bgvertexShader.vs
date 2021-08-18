struct VS_OUTPUT
{
	float4 Pos : SV_POSITION;
};

VS_OUTPUT VS(float4 inPos : POSITION)
{
	VS_OUTPUT output;

	output.Pos = inPos;
	return output;
}
