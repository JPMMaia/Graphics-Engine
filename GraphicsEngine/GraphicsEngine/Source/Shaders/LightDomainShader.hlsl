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
	float EdgeTesselationFactor[3]	: SV_TessFactor;
	float InsideTesselationFactor : SV_InsideTessFactor;
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

#define NUM_CONTROL_POINTS 3

[domain("tri")]
DomainOutput main(
	TesselationPatch input,
	float3 domain : SV_DomainLocation,
	const OutputPatch<HullOutput, NUM_CONTROL_POINTS> patch)
{
	DomainOutput output;

	// Interpolate values:
	output.PositionW = domain.x*patch[0].PositionW + domain.y*patch[1].PositionW + domain.z*patch[2].PositionW;
	output.NormalW = domain.x*patch[0].NormalW + domain.y*patch[1].NormalW + domain.z*patch[2].NormalW;
	output.TangentW = domain.x*patch[0].TangentW + domain.y*patch[1].TangentW + domain.z*patch[2].TangentW;
	output.TextureCoordinate = domain.x*patch[0].TextureCoordinate + domain.y*patch[1].TextureCoordinate + domain.z*patch[2].TextureCoordinate;

	// Calculate the mip level based on a mip interval:
	const float mipInterval = 15.0f;
	float mipLevel = clamp((distance(output.PositionW, g_eyePositionW) - mipInterval) / mipInterval, 0.0f, 6.0f);

	// Sample height map:
	float heightMapSample = g_heightMap.SampleLevel(g_samplerState, output.TextureCoordinate, mipLevel).x;

	// Ofset vertex along the normal by the sampled height map value:
	output.PositionW += 0.1f * (1.0f - heightMapSample) * output.NormalW;

	// Transform to homogeneous clip space:
	output.PositionH = mul(float4(output.PositionW, 1.0f), g_viewProjectionMatrix);

	return output;
}
