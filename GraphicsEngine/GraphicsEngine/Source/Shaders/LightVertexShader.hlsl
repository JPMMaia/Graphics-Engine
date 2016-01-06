#include <Common/Material.hlsli>
#include <Common/DirectionalLight.hlsli>

cbuffer CameraConstantBuffer : register(b0)
{
	float4x4 g_viewProjectionMatrix;
};

struct VertexInput
{
	float3 PositionL : POSITION;
	float3 NormalL : NORMAL;
	float2 TextureCoordinate : TEXCOORD0;
	row_major float4x4 WorldMatrix : WORLD;
};

struct VertexOutput
{
	float4 PositionH : SV_POSITION;
	float3 PositionW : POSITION;
	float3 NormalW : NORMAL;
	float2 TextureCoordinate : TEXCOORD0;
};

VertexOutput main(VertexInput input)
{
	VertexOutput output;

	// Transform to world space:
	output.PositionW = mul(float4(input.PositionL, 1.0f), input.WorldMatrix).xyz;
	output.NormalW = mul(input.NormalL, (float3x3)input.WorldMatrix); // TODO multiply by world inverse transpose

	// Transform to homogeneous clip space:
	output.PositionH = mul(float4(output.PositionW, 1.0f), g_viewProjectionMatrix);

	// Output texture coordinate for interpolation:
	output.TextureCoordinate = input.TextureCoordinate;

	return output;
}