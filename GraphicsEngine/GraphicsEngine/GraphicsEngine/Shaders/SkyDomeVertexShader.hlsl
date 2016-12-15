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
    float TextureCoordinates : TEXCOORD0;
};

VertexOutput main(VertexInput vertexInput, InstanceInput instanceInput)
{

    VertexOutput output;

    float3 positionW = EyePositionW + vertexInput.PositionL;
    output.PositionH = mul(float4(positionW, 1.0f), ViewProjectionMatrix);
    output.TextureCoordinates = (vertexInput.PositionL.y + 1.0f) / 2.0f;

    return output;
}
