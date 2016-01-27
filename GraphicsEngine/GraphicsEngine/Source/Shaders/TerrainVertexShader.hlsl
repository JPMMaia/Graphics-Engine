struct VertexInput
{
	float3 PositionL : POSITION;
	float2 TextureCoordinate : TEXCOORD0;
};

struct VertexOutput
{
	float3 PositionW : POSITION;
	float2 TextureCoordinate : TEXCOORD0;
};

VertexOutput main(VertexInput input)
{
	VertexOutput output;

	output.PositionW = input.PositionL;
	output.TextureCoordinate = input.TextureCoordinate;

	return output;
}