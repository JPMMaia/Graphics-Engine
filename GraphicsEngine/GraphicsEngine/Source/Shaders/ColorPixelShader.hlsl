struct VertexOutput
{
	float4 PositionH : SV_POSITION;
	float4 Color : COLOR;
};

float4 main(VertexOutput input) : SV_TARGET
{
	return input.Color;
}