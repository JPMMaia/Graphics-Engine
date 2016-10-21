#include "LightingUtils.hlsli"
#include "ObjectData.hlsli"
#include "MaterialData.hlsli"
#include "PassData.hlsli"

struct VertexInput
{
    float3 PositionL : POSITION;
    float3 NormalL : NORMAL;
    float2 TextureCoordinates : TEXCOORD;
};

struct VertexOutput
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
    float2 TextureCoordinates : TEXCOORD;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;

    // Transform position from local space to world space:
    float4 positionW = mul(float4(input.PositionL, 1.0f), WorldMatrix);
    output.PositionW = positionW.xyz;

    // Transform normal from local space to world space (assuming non-uniform scaling):
    output.NormalW = mul(input.NormalL, (float3x3) WorldMatrix);

    // Transform position to homogeneous clip space:
    output.PositionH = mul(positionW, ViewProjectionMatrix);

    // Output texture coordinates:
    output.TextureCoordinates = mul(float4(input.TextureCoordinates, 0.0f, 1.0f), MaterialTransform).xy;

    return output;
}
