#include "LightingUtils.hlsli"
#include "ObjectData.hlsli"
#include "MaterialData.hlsli"
#include "PassData.hlsli"

struct VertexInput
{
    float3 PositionL : POSITION;
    float3 NormalL : NORMAL;
    float2 TextureCoordinates : TEXCOORD;
    float3 TangentU : TANGENT;
};
struct InstanceInput
{
    row_major float4x4 WorldMatrix : WORLD;
    uint InstanceID : SV_InstanceID;
};

struct VertexOutput
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
    float2 TextureCoordinates : TEXCOORD;
};

VertexOutput main(VertexInput vertexInput, InstanceInput instanceInput)
{
    VertexOutput output;

    // Transform position from local space to world space:
    float4 positionW = mul(float4(vertexInput.PositionL, 1.0f), instanceInput.WorldMatrix);
    output.PositionW = positionW.xyz;

    // Transform normal from local space to world space (assuming non-uniform scaling):
    output.NormalW = mul(vertexInput.NormalL, (float3x3) instanceInput.WorldMatrix);

    // Transform position to homogeneous clip space:
    output.PositionH = mul(positionW, ViewProjectionMatrix);

    // Output texture coordinates:
    output.TextureCoordinates = mul(float4(vertexInput.TextureCoordinates, 0.0f, 1.0f), MaterialTransform).xy;

    return output;
}
