#include "LightingUtils.hlsli"
#include "MaterialData.hlsli"
#include "PassData.hlsli"
#include "Samplers.hlsli"
#include "MathUtils.hlsli"

struct DomainOutput
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float2 TextureCoordinates : TEXCOORD0;
    float2 TiledTextureCoordinates : TEXCOORD1;
};

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D HeightMap : register(t2);

float4 main(DomainOutput input) : SV_TARGET
{
    // Compute diffuse albedo by multiplying the sample from the texture and the diffuse albedo of the material:
    float4 diffuseAlbedo = DiffuseMap.Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates) * DiffuseAlbedo;

    // Calculate the normal, tangent and bitangent from the height map:
    float3 normalW, tangentW, bitangentW;
    CalculateNormalTangentBitangentFromHeightMap(input.TextureCoordinates, TexelSize, HeightMap, SamplerLinearClamp, normalW, tangentW, bitangentW);

    float3 normalSample = NormalMap.Sample(SamplerAnisotropicWrap, input.TiledTextureCoordinates);
    float3 bumpedNormalW = NormalSampleToBumpedNormalW(normalSample, normalW, tangentW);

    // Calculate direction from point to camera:
    float3 toEyeDirection = normalize(EyePositionW - input.PositionW);

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

    // Compute contribution of lights:
    float4 lightIntensity = ComputeLighting(Lights, material, input.PositionW, bumpedNormalW, toEyeDirection);
    
    // The final color results from the sum of the indirect and direct light:
    float4 color = ambientIntensity + lightIntensity;

    // Common convention to take alpha from diffuse albedo:
    color.a = diffuseAlbedo.a;

    return color;
}
