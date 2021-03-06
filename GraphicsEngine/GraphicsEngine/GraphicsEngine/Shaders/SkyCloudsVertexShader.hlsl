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

struct VertexOutput
{
    float4 PositionH : SV_POSITION;
    float2 TextureCoordinates : TEXCOORD0;
};

VertexOutput main(VertexInput vertexInput)
{
    VertexOutput output;

    float3 positionW = EyePositionW + mul(float4(vertexInput.PositionL, 1.0f), SkyCloudsPlaneTransformMatrix).xyz;
    output.PositionH = mul(float4(positionW, 1.0f), ViewProjectionMatrix);
    output.TextureCoordinates = vertexInput.TextureCoordinates;

    return output;
}
