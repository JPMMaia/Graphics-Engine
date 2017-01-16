#include "LightingUtils.hlsli"
#include "ObjectData.hlsli"
#include "MaterialData.hlsli"
#include "PassData.hlsli"

struct VertexInput
{
    float3 PositionL : POSITION;
    float3 NormalL : NORMAL;
    float2 TextureCoordinates : TEXCOORD;
    float3 TangentL : TANGENT;
};
struct InstanceInput
{
    row_major float4x4 WorldMatrix : WORLD;
    uint InstanceID : SV_InstanceID;
};

struct VertexOutput
{
    float3 PositionW : POSITION;
    float2 TextureCoordinates : TEXCOORD;
};

VertexOutput main(VertexInput vertexInput, InstanceInput instanceInput)
{
    VertexOutput output;

    // World position is equal to the local position:
    output.PositionW = vertexInput.PositionL;

    // Output texture coordinates:
    output.TextureCoordinates = mul(float4(vertexInput.TextureCoordinates, 0.0f, 1.0f), MaterialTransform).xy;

    return output;
}
