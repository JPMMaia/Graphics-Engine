#include "Samplers.hlsli"

struct VertexOutput
{
    float4 PositionH : SV_POSITION;
    float2 TextureCoordinates : TEXCOORD;
};

Texture2D TextureMap : register(t0);

float4 main(VertexOutput input) : SV_Target
{
    float3 color;

#if defined(SINGLE_CHANNEL)
    color = TextureMap.SampleLevel(SamplerAnisotropicClamp, input.TextureCoordinates, 0).rrr;
#elif defined(NORMALIZED_VECTORS)
    color = (TextureMap.SampleLevel(SamplerAnisotropicClamp, input.TextureCoordinates, 0).rgb + 1.0f) / 2.0f; 
#elif defined(HEIGHT_MAP)
    color = TextureMap.SampleLevel(SamplerAnisotropicClamp, input.TextureCoordinates, 0).rrr / 256.0f;
#endif

    return float4(color, 1.0f);
}