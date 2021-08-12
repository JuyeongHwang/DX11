VS_OUTPUT VS(float4 inPos : POSITION, float4 inColor : COLOR)
{
    VS_OUTPUT output;

    output.Pos = inPos;
    output.Color = inColor;

    return output;
}