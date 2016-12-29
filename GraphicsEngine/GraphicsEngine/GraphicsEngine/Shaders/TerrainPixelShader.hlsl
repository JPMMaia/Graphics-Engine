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
Texture2D NormalMap : register(t1);
Texture2D HeightMap : register(t2);
Texture2D ShadowMap : register(t3);
Texture2D TangentMap : register(t4);
Texture2D TiledDiffuseMap : register(t5);
Texture2D TiledNormalMap : register(t6);

float4 main(DomainOutput input) : SV_TARGET
{
    // Compute diffuse albedo by multiplying the sample from the texture and the diffuse albedo of the material:
    float4 diffuseAlbedo = TiledDiffuseMap.Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates) * DiffuseAlbedo;

    // Sample normal and tangent:
    float3 normalW = NormalMap.Sample(SamplerAnisotropicWrap, input.TextureCoordinates).rgb;
    float3 tangentW = TangentMap.Sample(SamplerAnisotropicWrap, input.TextureCoordinates).rgb;

    // Sample value from the tiled normal map and compute the bumped normal in world space:
    float3 tiledNormalSample = TiledNormalMap.Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates).rgb;
    float3 bumpedTiledNormalW = NormalSampleToBumpedNormalW(tiledNormalSample, normalW, tangentW);
    bumpedTiledNormalW = normalize(bumpedTiledNormalW);
    //float3 bumpedTiledNormalW = normalW;

    /*float4 rockDiffuseAlbedo = float4(0.0f, 0.0f, 0.0f, 1.0f);
    float4 snowDiffuseAlbedo = float4(1.0f, 1.0f, 1.0f, 1.0f);

    float4 diffuseAlbedo;
    float slope = 1.0f - bumpedTiledNormalW.y;
    if (slope < 0.2f)
    {
        float blendAmount = slope / 0.2f;
        diffuseAlbedo = lerp(snowDiffuseAlbedo, rockDiffuseAlbedo, blendAmount);
    }
    if (slope >= 0.2f)
    {
        diffuseAlbedo = rockDiffuseAlbedo;
    }
    diffuseAlbedo *= DiffuseAlbedo;*/

    // Calculate direction from point to camera:
    float3 toEyeDirection = EyePositionW - input.PositionW;
    float distanceToEye = length(toEyeDirection);
    toEyeDirection /= distanceToEye;

    // Calculate indirect lighting:
    float4 ambientIntensity = AmbientLight * diffuseAlbedo;

    // Create a material:
    float shininess = 1.0f - Roughness;
    Material material =
    {
        diffuseAlbedo,
        FresnelR0,
        shininess
    };

    // Calculate the shadow factor:
    float shadowFactor = CalculateShadowFactor(ShadowMap, SamplerShadows, input.ShadowPositionH);

    // Compute contribution of lights:
    float4 lightIntensity = ComputeLighting(Lights, material, input.PositionW, bumpedTiledNormalW, toEyeDirection, shadowFactor);
    
    // The final color results from the sum of the indirect and direct light:
    float4 color = ambientIntensity + lightIntensity;

#if defined(FOG)
    color = AddFog(color, distanceToEye, FogStart, FogRange, FogColor);
#endif

    // Common convention to take alpha from diffuse albedo:
    color.a = diffuseAlbedo.a;

    return color;
}
