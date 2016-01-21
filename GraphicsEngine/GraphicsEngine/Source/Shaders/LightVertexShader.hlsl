#include <Common/Material.hlsli>
#include <Common/DirectionalLight.hlsli>

struct VertexInput
{
	float3 PositionL : POSITION;	
	float3 NormalL : NORMAL;
	float3 TangentL : TANGENT;
	float2 TextureCoordinate : TEXCOORD0;
	row_major float4x4 WorldMatrix : WORLD;
};

struct VertexOutput
{
	float3 PositionW : POSITION;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 TextureCoordinate : TEXCOORD0;
};

VertexOutput main(VertexInput input)
{
	VertexOutput output;

	// Transform to world space:
	output.PositionW = mul(float4(input.PositionL, 1.0f), input.WorldMatrix).xyz;
	output.NormalW = mul(input.NormalL, (float3x3)input.WorldMatrix); // TODO multiply by world inverse transpose
	output.TangentW = mul(input.TangentL, (float3x3)input.WorldMatrix);

	// Output texture coordinate for interpolation:
	output.TextureCoordinate = input.TextureCoordinate;

	return output;
}