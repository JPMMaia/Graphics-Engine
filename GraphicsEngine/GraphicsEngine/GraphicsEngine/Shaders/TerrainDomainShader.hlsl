#define NUM_CONTROL_POINTS 4
#define NUM_TESSELATION_FACTOR 4
#define NUM_INSIDE_TESSELATION_FACTOR 2

#include "LightingUtils.hlsli"
#include "PassData.hlsli"
#include "Samplers.hlsli"
#include "MathUtils.hlsli"

struct HullOutput
{
    float3 PositionW : POSITION;
    float2 TextureCoordinates : TEXCOORD;
};

struct DomainOutput
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float2 TextureCoordinates : TEXCOORD;
};

struct TesselationPatch
{
    float EdgeTesselationFactor[NUM_TESSELATION_FACTOR] : SV_TessFactor;
    float InsideTesselationFactor[NUM_INSIDE_TESSELATION_FACTOR] : SV_InsideTessFactor;
};

Texture2D HeightMap : register(t2);

[domain("quad")]
DomainOutput main(TesselationPatch input, float2 uv : SV_DomainLocation, const OutputPatch<HullOutput, NUM_CONTROL_POINTS> patch)
{
    DomainOutput output;

	// Interpolate values:
    output.PositionW = BilinearInterpolate(patch[0].PositionW, patch[1].PositionW, patch[2].PositionW, patch[3].PositionW, uv);
    output.TextureCoordinates = BilinearInterpolate(patch[0].TextureCoordinates, patch[1].TextureCoordinates, patch[2].TextureCoordinates, patch[3].TextureCoordinates, uv);

	// Sample height map:
    output.PositionW.y = 512.0f * HeightMap.SampleLevel(SamplerAnisotropicClamp, output.TextureCoordinates, 0).r;

	// Transform to homogeneous clip space:
    output.PositionH = mul(float4(output.PositionW, 1.0f), ViewProjectionMatrix);

    return output;
}