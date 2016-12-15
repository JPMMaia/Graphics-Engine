#include "LightingUtils.hlsli"
#include "MaterialData.hlsli"
#include "PassData.hlsli"
#include "Samplers.hlsli"

struct VertexOutput
{
    float4 PositionH : SV_POSITION;
    float TextureCoordinates : TEXCOORD0;
};

float4 main(VertexOutput input) : SV_TARGET
{
    float s = saturate(input.TextureCoordinates);
    float4 outputColor = lerp(SkyDomeColors[0], SkyDomeColors[1], s);
    
    return outputColor;
}
