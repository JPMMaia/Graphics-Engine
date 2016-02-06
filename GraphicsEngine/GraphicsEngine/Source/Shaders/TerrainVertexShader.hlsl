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

SamplerState g_samplerState : register(s0);
Texture2D g_heightMap : register(t0);

VertexOutput main(VertexInput input)
{
	VertexOutput output;

	output.PositionW = input.PositionL;
	output.TextureCoordinate = input.TextureCoordinate;
	
	output.PositionW.y = 5.0f * g_heightMap.SampleLevel(g_samplerState, output.TextureCoordinate, 0).r;

	return output;
}