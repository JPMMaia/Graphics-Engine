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
    float blendFactor = saturate(input.TextureCoordinates);
    float4 color = lerp(SkyDomeColors[0], SkyDomeColors[1], blendFactor);

#if defined(FOG)
    color = lerp(color, FogColor, FogColor.w);
#endif

    color.a = 1.0f;

    return color;
}
