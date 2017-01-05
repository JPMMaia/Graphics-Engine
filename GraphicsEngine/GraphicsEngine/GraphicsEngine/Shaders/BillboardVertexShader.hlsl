#include "LightingUtils.hlsli"
#include "PassData.hlsli"

struct VertexInput
{
    float3 CenterW : POSITION;
    float2 ExtentsW : TEXCOORD;
};
struct VertexOutput
{
    float3 CenterW : POSITION;
    float2 ExtentsW : TEXCOORD0;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;

    output.CenterW = input.CenterW;
    output.ExtentsW = input.ExtentsW;

    return output;
}