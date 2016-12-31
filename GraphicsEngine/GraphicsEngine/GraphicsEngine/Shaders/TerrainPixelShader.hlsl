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

Texture2D DiffuseMap : register(t0);
Texture2D<float3> NormalMap : register(t1);
Texture2D HeightMap : register(t2);
Texture2D ShadowMap : register(t3);
Texture2D<float3> TangentMap : register(t4);
Texture2D TiledDiffuseMap : register(t5);
Texture2D TiledNormalMap : register(t6);
Texture2D TiledNormalMap2 : register(t7);

float4 main(DomainOutput input) : SV_TARGET
{
    // Compute diffuse albedo by multiplying the sample from the texture and the diffuse albedo of the material:
    float4 rockDiffuseAlbedo = TiledDiffuseMap.Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates) * float4(0.26f, 0.30f, 0.38f, 1.0f);
    float4 snowDiffuseAlbedo = float4(1.0f, 1.0f, 1.0f, 1.0f);

    // Create rock material:
    Material rockMaterial =
    {
        rockDiffuseAlbedo,
        float3(0.2f, 0.2f, 0.2f),
        0.1f
    };
    
    // Create snow material:
    Material snowMaterial =
    {
        snowDiffuseAlbedo,
        float3(0.8f, 0.8f, 0.8f),
        0.7f
    };

    // Sample normal and tangent:
    float3 normalW = NormalMap.Sample(SamplerAnisotropicWrap, input.TextureCoordinates).rgb;
    float3 tangentW = TangentMap.Sample(SamplerAnisotropicWrap, input.TextureCoordinates).rgb;

    // Sample value from the tiled normal map and compute the bumped normal in world space:
    float3 rockNormalSample = TiledNormalMap.Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates).rgb;
    float3 rockNormalW = NormalSampleToBumpedNormalW(rockNormalSample, normalW, tangentW);
    //float3 bumpedTiledNormalW = normalize(normalW);
    float3 snowNormalSample = TiledNormalMap2.Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates).rgb;
    float3 snowNormalW = NormalSampleToBumpedNormalW(snowNormalSample, normalW, tangentW);

    // Calculate direction from point to camera:
    float3 toEyeDirection = EyePositionW - input.PositionW;
    float distanceToEye = length(toEyeDirection);
    toEyeDirection /= distanceToEye;

    // Calculate the shadow factor:
    float shadowFactor = CalculateShadowFactor(ShadowMap, SamplerShadows, input.ShadowPositionH);

    // Compute contribution of lights:
    float4 rockLightIntensity = ComputeLighting(Lights, rockMaterial, input.PositionW, rockNormalW, toEyeDirection, shadowFactor);
    float4 snowLightIntensity = ComputeLighting(Lights, snowMaterial, input.PositionW, snowNormalW, toEyeDirection, shadowFactor);
    
    // Compute indirect light:
    float4 rockAmbientIntensity = AmbientLight * rockDiffuseAlbedo;
    float4 snowAmbientIntensity = AmbientLight * snowDiffuseAlbedo;

    // The final color results from the sum of the indirect and direct light:
    float4 rockColor = rockAmbientIntensity + rockLightIntensity;
    float4 snowColor = snowAmbientIntensity + snowLightIntensity;

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
