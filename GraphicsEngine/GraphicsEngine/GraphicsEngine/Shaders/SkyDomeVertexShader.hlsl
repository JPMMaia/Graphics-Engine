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
    float BlendFactor : TEXCOORD0;
};

VertexOutput main(VertexInput vertexInput)
{
    VertexOutput output;

    float3 positionW = EyePositionW + vertexInput.PositionL;
    output.PositionH = mul(float4(positionW, 1.0f), ViewProjectionMatrix);
    output.BlendFactor = (vertexInput.PositionL.y + 1.0f) / 2.0f;

    return output;
}
