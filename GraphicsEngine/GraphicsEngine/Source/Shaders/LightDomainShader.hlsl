#include <Common/MathUtils.hlsli>

cbuffer CameraConstantBuffer : register(b0)
{
	float4x4 g_viewProjectionMatrix;
	float3 g_eyePositionW;
};

struct HullOutput
{
	float3 PositionW : POSITION;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 TextureCoordinate : TEXCOORD0;
};

struct TesselationPatch
{
	float EdgeTesselationFactor[4]		: SV_TessFactor;
	float InsideTesselationFactor[2]	: SV_InsideTessFactor;
};

struct DomainOutput
{
	float4 PositionH : SV_POSITION;
	float3 PositionW : POSITION;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 TextureCoordinate : TEXCOORD0;
};

SamplerState g_samplerState : register(s0);
Texture2D g_heightMap : register(t2);

#define NUM_CONTROL_POINTS 4

[domain("quad")]
DomainOutput main(
	TesselationPatch input,
	float2 domain : SV_DomainLocation,
	const OutputPatch<HullOutput, NUM_CONTROL_POINTS> patch)
{
	DomainOutput output;

	// Interpolate values:
	output.PositionW = BilinearInterpolate(patch[0].PositionW, patch[1].PositionW, patch[2].PositionW, patch[3].PositionW, domain);
	output.NormalW = BilinearInterpolate(patch[0].NormalW, patch[1].NormalW, patch[2].NormalW, patch[3].NormalW, domain);
	output.TangentW = BilinearInterpolate(patch[0].TangentW, patch[1].TangentW, patch[2].TangentW, patch[3].TangentW, domain);
	output.TextureCoordinate = BilinearInterpolate(patch[0].TextureCoordinate, patch[1].TextureCoordinate, patch[2].TextureCoordinate, patch[3].TextureCoordinate, domain);

	// Calculate the mip level based on a mip interval:
	const float mipInterval = 15.0f;
	float mipLevel = clamp((distance(output.PositionW, g_eyePositionW) - mipInterval) / mipInterval, 0.0f, 6.0f);

	// Sample height map:
	float heightMapSample = g_heightMap.SampleLevel(g_samplerState, output.TextureCoordinate, mipLevel).x;

	// Offset vertex along the normal by the sampled height map value:
	output.NormalW = normalize(output.NormalW);
	//output.PositionW += 0.1f * (1.0f - heightMapSample) * output.NormalW;

	// Transform to homogeneous clip space:
	output.PositionH = mul(float4(output.PositionW, 1.0f), g_viewProjectionMatrix);

	return output;
}