#include "LightingUtils.hlsli"
#include "MaterialData.hlsli"
#include "PassData.hlsli"
#include "Samplers.hlsli"


struct VertexOutput
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
};

float4 main(VertexOutput input) : SV_TARGET
{
    float height;
    float4 outputColor;

    height = input.PositionW.y;

    if (height < 0.0)
        height = 0.0f;

    outputColor = lerp(SkyDomeColors[0], SkyDomeColors[1], height);
    
    return outputColor;
}
