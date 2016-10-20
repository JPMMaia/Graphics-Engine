#include "LightingUtils.hlsli"

#include "ObjectData.hlsli"
#include "MaterialData.hlsli"
#include "PassData.hlsli"

struct VertexInput
{
    float3 PositionL : POSITION;
    float3 NormalL : NORMAL;
};

struct VertexOutput
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;

    // Transform from local space to world space:
    float4 positionW = mul(float4(input.PositionL, 1.0f), WorldMatrix);
    output.PositionW = positionW.xyz;

    // Assuming non-uniform scaling:
    output.NormalW = mul(input.NormalL, (float3x3) WorldMatrix);

    // Transform to homogeneous clip space:
    output.PositionH = mul(positionW, ViewProjectionMatrix);
    
    return output;
}
