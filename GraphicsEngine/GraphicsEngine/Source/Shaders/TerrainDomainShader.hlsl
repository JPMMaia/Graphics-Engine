#include <Common/MathUtils.hlsli>

#define NUM_CONTROL_POINTS 4
#define NUM_TESSELATION_FACTOR 4
#define NUM_INSIDE_TESSELATION_FACTOR 2

cbuffer CameraConstantBuffer : register(b0)
{
	float4x4 g_viewProjectionMatrix;
	float3 g_eyePositionW;
};

struct HullOutput
{
	float3 PositionW : POSITION;
	float2 TextureCoordinate : TEXCOORD0;
};

struct DomainOutput
{
	float4 PositionH : SV_POSITION;
	float3 PositionW : POSITION;
	float2 TextureCoordinate : TEXCOORD0;
};

struct TesselationPatch
{
	float EdgeTesselationFactor[NUM_TESSELATION_FACTOR]				: SV_TessFactor;
	float InsideTesselationFactor[NUM_INSIDE_TESSELATION_FACTOR]	: SV_InsideTessFactor;
};

SamplerState g_samplerState : register(s0);
Texture2D g_heightMap : register(t0);

[domain("quad")]
DomainOutput main(TesselationPatch input, float2 domain : SV_DomainLocation, const OutputPatch<HullOutput, NUM_CONTROL_POINTS> patch)
{
	DomainOutput output;

	// Interpolate values:
	output.PositionW = BilinearInterpolate(patch[0].PositionW, patch[1].PositionW, patch[2].PositionW, patch[3].PositionW, domain);
	output.TextureCoordinate = BilinearInterpolate(patch[0].TextureCoordinate, patch[1].TextureCoordinate, patch[2].TextureCoordinate, patch[3].TextureCoordinate, domain);

	// Sample height map:
	output.PositionW.y = 5.0f * g_heightMap.SampleLevel(g_samplerState, output.TextureCoordinate, 0).r;

	// Transform to homogeneous clip space:
	output.PositionH = mul(float4(output.PositionW, 1.0f), g_viewProjectionMatrix);

	return output;
}
