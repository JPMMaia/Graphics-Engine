#include <Common/Material.hlsli>
#include <Common/DirectionalLight.hlsli>

cbuffer CameraConstantBuffer : register(b0)
{
	float4x4 g_viewProjectionMatrix;
	float3 g_eyePositionW;
};

cbuffer TesselationConstantBuffer : register(b1)
{
	float g_maxTesselationDistance;
	float g_minTesselationDistance;
	float g_maxTesselationFactor;
	float g_minTesselationFactor;
}

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
	float TesselationFactor : TESSFACTOR;
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

	// Calculate the distance from the vertex to the camera:
	float toEyeDistance = distance(g_eyePositionW, output.PositionW);

	//
	// Calculate tesselation factor
	//	If toEyeDistance in [0, max_tesselation_distance], then tesselation_factor = min_tesselation_factor
	//	If toEyeDistance in ]max_tesselation_distance, min_tesselation_distance[, then tesselation_factor varies linearly in [min_tesselation_factor, max_tesselation_factor]
	//	If toEyeDistance in [min_tesselation_distance, plus_infinity], then tesselation_factor = max_tesselation_factor
	//
	float tesselationDistanceScalar = saturate((g_minTesselationDistance - toEyeDistance) / (g_minTesselationDistance - g_maxTesselationDistance));
	output.TesselationFactor = g_minTesselationFactor + tesselationDistanceScalar*(g_maxTesselationFactor - g_minTesselationFactor);

	return output;
}