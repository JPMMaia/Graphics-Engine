#include <Common/Material.hlsli>
#include <Common/DirectionalLight.hlsli>

cbuffer PerObjectConstantBuffer : register(b0)
{
	float4x4 g_worldMatrix;
	float4x4 g_worldInverseTransposeMatrix;
	float4x4 g_worldViewProjectionMatrix;
};

struct VertexInput
{
	float3 PositionL : POSITION;
	float3 NormalL : NORMAL;
	float2 TextureCoordinate : TEXCOORD0;
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

	float4 positionL = float4(input.PositionL, 1.0f);

	// Transform to world space:
	output.PositionW = mul(positionL, g_worldMatrix).xyz;
	output.NormalW = mul(input.NormalL, (float3x3)g_worldInverseTransposeMatrix);

	// Transform to homogeneous clip space:
	output.PositionH = mul(positionL, g_worldViewProjectionMatrix);

	// Output texture coordinate for interpolation:
	output.TextureCoordinate = input.TextureCoordinate;

	return output;
}