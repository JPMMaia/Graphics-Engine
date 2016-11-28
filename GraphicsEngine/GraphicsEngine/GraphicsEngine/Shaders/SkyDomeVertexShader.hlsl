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
};

VertexOutput main(VertexInput vertexInput, InstanceInput instanceInput)
{

    VertexOutput output;

    
    output.PositionW = vertexInput.PositionL.xyz;

    // Transform position to homogeneous clip space:
    float4 positionW = mul(float4(vertexInput.PositionL, 1.0f), instanceInput.WorldMatrix);
    output.PositionH = mul(positionW + float4(EyePositionW,0.0f), ViewProjectionMatrix);

    return output;
}
