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
Texture2D RockMaps[3] : register(t4);
Texture2D GrassMaps[3] : register(t7);
Texture2D PathMaps[4] : register(t10);
Texture2D SnowMaps[1] : register(t14);

float4 ComputeColor(float4 diffuseAlbedo, Material material, Texture2D normalMap, float3 positionW, float2 tiledTextureCoordinates, float3 normalW, float3 tangentW, float3 toEyeDirection, float shadowFactor, float specularFactor)
{
    // Sample value from the tiled normal map and compute the bumped normal in world space:
    float3 normalSample = normalMap.Sample(SamplerAnisotropicWrap, tiledTextureCoordinates).rgb;
    float3 bumpedNormalW = NormalSampleToBumpedNormalW(normalSample, normalW, tangentW);

    // Compute contribution of light:
    float4 lightIntensity = ComputeLighting(Lights, material, positionW, bumpedNormalW, toEyeDirection, shadowFactor, specularFactor);

    // Compute indirect light:
    float4 ambientIntensity = AmbientLight * diffuseAlbedo;

    // The final color results from the sum of the indirect and direct light:
    return ambientIntensity + lightIntensity;
}

float4 ComputeRockColor(float3 positionW, float2 tiledTextureCoordinates, float3 normalW, float3 tangentW, float3 toEyeDirection, float shadowFactor)
{
    // Compute diffuse albedo by multiplying the sample from the texture and the diffuse albedo of the material:
    float4 diffuseAlbedo = RockMaps[0].Sample(SamplerAnisotropicWrap, tiledTextureCoordinates) * float4(0.26f, 0.30f, 0.38f, 1.0f);

    // Create material:
    Material material =
    {
        diffuseAlbedo,
        float3(0.2f, 0.2f, 0.2f),
        0.1f
    };

    // Sample specular factor:
    float specularFactor = RockMaps[2].Sample(SamplerAnisotropicWrap, tiledTextureCoordinates).r;

    return ComputeColor(diffuseAlbedo, material, RockMaps[1], positionW, tiledTextureCoordinates, normalW, tangentW, toEyeDirection, shadowFactor, specularFactor);
}

float4 ComputeGrassColor(float3 positionW, float2 tiledTextureCoordinates, float3 normalW, float3 tangentW, float3 toEyeDirection, float shadowFactor)
{
    // Sample value from diffuse map:
    float4 diffuseAlbedo = GrassMaps[0].Sample(SamplerAnisotropicWrap, tiledTextureCoordinates);
    
    // Create material:
    Material material =
    {
        diffuseAlbedo,
        float3(0.5f, 0.5f, 0.5f),
        0.1f
    };

    // Sample specular factor:
    float specularFactor = GrassMaps[2].Sample(SamplerAnisotropicWrap, tiledTextureCoordinates).r;

    return ComputeColor(diffuseAlbedo, material, GrassMaps[1], positionW, tiledTextureCoordinates, normalW, tangentW, toEyeDirection, shadowFactor, specularFactor);
}

float4 ComputePathColor(float3 positionW, float2 tiledTextureCoordinates, float3 normalW, float3 tangentW, float3 toEyeDirection, float shadowFactor)
{
    // Sample value from diffuse map:
    float4 diffuseAlbedo = PathMaps[0].Sample(SamplerAnisotropicWrap, tiledTextureCoordinates);
    
    // Create material:
    Material material =
    {
        diffuseAlbedo,
        float3(0.5f, 0.5f, 0.5f),
        0.1f
    };

    // Sample specular factor:
    float specularFactor = PathMaps[2].Sample(SamplerAnisotropicWrap, tiledTextureCoordinates).r;

    return ComputeColor(diffuseAlbedo, material, PathMaps[1], positionW, tiledTextureCoordinates, normalW, tangentW, toEyeDirection, shadowFactor, specularFactor);
}

float4 ComputeSnowColor(float3 positionW, float2 tiledTextureCoordinates, float3 normalW, float3 tangentW, float3 toEyeDirection, float shadowFactor)
{
    // Set diffuse albedo:
    float4 diffuseAlbedo = float4(1.0f, 1.0f, 1.0f, 1.0f);
    
    // Create material:
    Material material =
    {
        diffuseAlbedo,
        float3(0.8f, 0.8f, 0.8f),
        0.7f
    };

    // Set specular factor:
    float specularFactor = 1.0f;

    return ComputeColor(diffuseAlbedo, material, SnowMaps[0], positionW, tiledTextureCoordinates, normalW, tangentW, toEyeDirection, shadowFactor, specularFactor);
}

float4 main(DomainOutput input) : SV_TARGET
{
#if defined(DEBUG_PATH_ALPHA)
    return float4(PathMaps[3].Sample(SamplerAnisotropicWrap, input.TextureCoordinates).rrr, 1.0f);
#endif

    // Calculate direction from point to camera:
    float3 toEyeDirection = EyePositionW - input.PositionW;
    float distanceToEye = length(toEyeDirection);
    toEyeDirection /= distanceToEye;

#if defined(FOG)
    float fogIntensity = saturate((distanceToEye - FogStart) / FogRange);
    if(fogIntensity - 0.99f > 0.0f)
        return FogColor;
#endif

    // Sample normal and tangent:
    float3 normalW = NormalMap.Sample(SamplerAnisotropicWrap, input.TextureCoordinates).rgb;
    float3 tangentW = TangentMap.Sample(SamplerAnisotropicWrap, input.TextureCoordinates).rgb;

    // Normalize sample vectors:
    normalW = normalize(normalW);
    tangentW = normalize(tangentW);

#if defined(DEBUG_NORMAL_VECTORS)
    return float4((normalW + 1.0f) / 2.0f, 1.0f);
#elif defined(DEBUG_TANGENT_VECTORS)
    return float4((tangentW + 1.0f) / 2.0f, 1.0f);
#endif

    // Calculate the shadow factor:
    float shadowFactor = CalculateShadowFactor(ShadowMap, SamplerShadows, input.ShadowPositionH);
    
#if defined(FOG)

    shadowFactor *= 1.0f - FogColor.w;

#endif

#if defined(DEBUG_NORMAL_MAPPING)
    float3 rockNormalSample = RockMaps[1].Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates).rgb;
    float3 rockBumpedNormalW = NormalSampleToBumpedNormalW(rockNormalSample, normalW, tangentW);
    float4 rockColor = float4((rockBumpedNormalW + 1.0f) / 2.0f, 1.0f);
#elif defined(DEBUG_SPECULAR_MAP)
    float4 rockColor = float4(RockMaps[2].Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates).rrr, 1.0f);
#else
    // Calculate the color for rock:
    float4 rockColor = ComputeRockColor(input.PositionW, input.TiledTextureCoordinates, normalW, tangentW, toEyeDirection, shadowFactor);
#endif

    // Interpolate materials depending on the slope and height:
    float4 color;
    float slope = 1.0f - normalW.y;
    if (slope < 0.2f)
    {
        float4 blendColor;

        // Interpolate between grass and snow, depending on height:
        const float up = 30.0f;
        const float low = 20.0f;
        if (input.PositionW.y < low)
        {
#if defined(DEBUG_NORMAL_MAPPING)
            float3 grassNormalSample = GrassMaps[1].Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates).rgb;
            float3 grassBumpedNormalW = NormalSampleToBumpedNormalW(grassNormalSample, normalW, tangentW);
            blendColor = float4((grassBumpedNormalW + 1.0f) / 2.0f, 1.0f);
#elif defined(DEBUG_SPECULAR_MAP)
            blendColor = float4(GrassMaps[2].Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates).rrr, 1.0f);
#else
            blendColor = ComputeGrassColor(input.PositionW, input.TiledTextureCoordinates, normalW, tangentW, toEyeDirection, shadowFactor);
#endif
        }
        else if (input.PositionW.y < up)
        {
#if defined(DEBUG_NORMAL_MAPPING)
            float3 grassNormalSample = GrassMaps[1].Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates).rgb;
            float3 grassBumpedNormalW = NormalSampleToBumpedNormalW(grassNormalSample, normalW, tangentW);
            float4 grassColor = float4((grassBumpedNormalW + 1.0f) / 2.0f, 1.0f);
            float3 snowNormalSample = SnowMaps[0].Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates).rgb;
            float3 snowBumpedNormalW = NormalSampleToBumpedNormalW(snowNormalSample, normalW, tangentW);
            float4 snowColor = float4((snowBumpedNormalW + 1.0f) / 2.0f, 1.0f);
#elif defined(DEBUG_SPECULAR_MAP)
            float4 grassColor = float4(GrassMaps[2].Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates).rrr, 1.0f);
            float4 snowColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
#else
            float4 grassColor = ComputeGrassColor(input.PositionW, input.TiledTextureCoordinates, normalW, tangentW, toEyeDirection, shadowFactor);
            float4 snowColor = ComputeSnowColor(input.PositionW, input.TiledTextureCoordinates, normalW, tangentW, toEyeDirection, shadowFactor);
#endif
            blendColor = lerp(snowColor, grassColor, (up - input.PositionW.y) / (up - low));
        }
        else
        {
#if defined(DEBUG_NORMAL_MAPPING)
            float3 snowNormalSample = SnowMaps[0].Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates).rgb;
            float3 snowBumpedNormalW = NormalSampleToBumpedNormalW(snowNormalSample, normalW, tangentW);
            blendColor = float4((snowBumpedNormalW + 1.0f) / 2.0f, 1.0f);
#elif defined(DEBUG_SPECULAR_MAP)
            blendColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
#else
            blendColor = ComputeSnowColor(input.PositionW, input.TiledTextureCoordinates, normalW, tangentW, toEyeDirection, shadowFactor);
#endif
        }

        // Interpolate color with rock, based on the slope:
        float blendAmount = slope / 0.2f;
        color = lerp(blendColor, rockColor, blendAmount);
    }
    else
    {
        color = rockColor;
    }

    float pathAlpha = PathMaps[3].Sample(SamplerAnisotropicWrap, input.TextureCoordinates).r;

#if defined(DEBUG_NORMAL_MAPPING)
    float3 pathNormalSample = PathMaps[1].Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates).rgb;
    float3 pathBumpedNormalW = NormalSampleToBumpedNormalW(pathNormalSample, normalW, tangentW);
    float4 pathColor = float4((pathBumpedNormalW + 1.0f) / 2.0f, 1.0f);
#elif defined(DEBUG_SPECULAR_MAP)
    float4 pathColor = float4(PathMaps[2].Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates).rrr, 1.0f);
#else
    float4 pathColor = ComputePathColor(input.PositionW, input.TiledTextureCoordinates, normalW, tangentW, toEyeDirection, shadowFactor);
#endif
    color = lerp(color, pathColor, pathAlpha);

#if defined(FOG)
    color = lerp(color, FogColor, fogIntensity);
#endif

    // Set alpha channel to 1:
    color.a = 1.0f;

    return color;
}
