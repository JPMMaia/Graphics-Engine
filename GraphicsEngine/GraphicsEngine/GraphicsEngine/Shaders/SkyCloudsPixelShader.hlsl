#include "LightingUtils.hlsli"
#include "MaterialData.hlsli"
#include "PassData.hlsli"
#include "Samplers.hlsli"

struct VertexOutput
{
    float4 PositionH : SV_POSITION;
    float2 TextureCoordinates : TEXCOORD0;
};

Texture2D CloudsMap : register(t0);
Texture2D NoiseMap : register(t1);

float4 main(VertexOutput input) : SV_TARGET
{
    // Translate texture coordinates:
    float2 textureCoordinates = input.TextureCoordinates + CloudsTranslation;

    // Sample the noise value from the noise map:
    float2 noiseValue = NoiseMap.Sample(SamplerAnisotropicWrap, textureCoordinates).xx;

    // Scale the noise value:
    float scale = 0.1f + cos(0.00005f * TotalTime) * 0.5f;
    noiseValue *= scale;

    // Apply noise to the translated texture coordinates:
    float2 noiseTextureCoordinates = input.TextureCoordinates + CloudsTranslation + noiseValue;

    // Sample color from clouds map using the noise texture coordinates:
    float4 color = CloudsMap.Sample(SamplerAnisotropicWrap, noiseTextureCoordinates);

    // Reduce brithness:
    float brightness = 0.5f;
    color *= brightness;

#if defined(FOG)
    color = lerp(color, FogColor, FogColor.w);
#endif

    return color;
}
