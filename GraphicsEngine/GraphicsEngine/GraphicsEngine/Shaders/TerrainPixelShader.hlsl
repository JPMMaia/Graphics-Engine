#include "LightingUtils.hlsli"
#include "MaterialData.hlsli"
#include "PassData.hlsli"
#include "Samplers.hlsli"
#include "MathUtils.hlsli"

struct DomainOutput
{
    float3 PositionW : POSITION0;
    float4 PositionH : SV_POSITION;
    float4 ShadowPositionH : POSITION1;
    float2 TextureCoordinates : TEXCOORD0;
    float2 TiledTextureCoordinates : TEXCOORD1;
};

Texture2D<float3> NormalMap : register(t0);
Texture2D HeightMap : register(t1);
Texture2D TangentMap : register(t2);
Texture2D ShadowMap : register(t3);
Texture2D RockDiffuseMap : register(t4);
Texture2D RockNormalMap : register(t5);
Texture2D GrassDiffuseMap : register(t6);
Texture2D GrassNormalMap : register(t7);
Texture2D PathDiffuseMap : register(t8);
Texture2D PathNormalMap : register(t9);
Texture2D SnowNormalMap : register(t10);

float4 ComputeColor(float4 diffuseAlbedo, Material material, Texture2D normalMap, float3 positionW, float2 tiledTextureCoordinates, float3 normalW, float3 tangentW, float3 toEyeDirection, float shadowFactor)
{
    // Sample value from the tiled normal map and compute the bumped normal in world space:
    float3 normalSample = normalMap.Sample(SamplerAnisotropicWrap, tiledTextureCoordinates).rgb;
    float3 bumpedNormalW = NormalSampleToBumpedNormalW(normalSample, normalW, tangentW);

    // Compute contribution of light:
    float4 lightIntensity = ComputeLighting(Lights, material, positionW, bumpedNormalW, toEyeDirection, shadowFactor);

    // Compute indirect light:
    float4 ambientIntensity = AmbientLight * diffuseAlbedo;

    // The final color results from the sum of the indirect and direct light:
    return ambientIntensity + lightIntensity;
}

float4 main(DomainOutput input) : SV_TARGET
{
    // Sample normal and tangent:
    float3 normalW = NormalMap.Sample(SamplerAnisotropicWrap, input.TextureCoordinates).rgb;
    float3 tangentW = TangentMap.Sample(SamplerAnisotropicWrap, input.TextureCoordinates).rgb;

    // Calculate direction from point to camera:
    float3 toEyeDirection = EyePositionW - input.PositionW;
    float distanceToEye = length(toEyeDirection);
    toEyeDirection /= distanceToEye;

    // Calculate the shadow factor:
    float shadowFactor = CalculateShadowFactor(ShadowMap, SamplerShadows, input.ShadowPositionH);
    
    // Rock:
    float4 rockColor;
    {
        // Compute diffuse albedo by multiplying the sample from the texture and the diffuse albedo of the material:
        float4 rockDiffuseAlbedo = RockDiffuseMap.Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates) * float4(0.26f, 0.30f, 0.38f, 1.0f);

        // Create rock material:
        Material rockMaterial =
        {
            rockDiffuseAlbedo,
            float3(0.2f, 0.2f, 0.2f),
            0.1f
        };

        rockColor = ComputeColor(rockDiffuseAlbedo, rockMaterial, RockNormalMap, input.PositionW, input.TiledTextureCoordinates, normalW, tangentW, toEyeDirection, shadowFactor);
    }

    // Snow:
    float4 snowColor;
    {
        float4 snowDiffuseAlbedo = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
        // Create snow material:
        Material snowMaterial =
        {
            snowDiffuseAlbedo,
            float3(0.8f, 0.8f, 0.8f),
            0.7f
        };

        snowColor = ComputeColor(snowDiffuseAlbedo, snowMaterial, SnowNormalMap, input.PositionW, input.TiledTextureCoordinates, normalW, tangentW, toEyeDirection, shadowFactor);
    }

    // Interpolate materials depending on the slope:
    float4 color;
    float slope = 1.0f - normalW.y;
    if (slope < 0.2f)
    {
        float blendAmount = slope / 0.2f;
        color = lerp(snowColor, rockColor, blendAmount);
    }
    else
    {
        color = rockColor;
    }

#if defined(FOG)
    color = AddFog(color, distanceToEye, FogStart, FogRange, FogColor);
#endif

    // Set alpha channel to 1:
    color.a = 1.0f;

    return color;
}
