#include "LightingUtils.hlsli"
#include "MaterialData.hlsli"
#include "PassData.hlsli"
#include "Samplers.hlsli"

struct DomainOutput
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float2 TextureCoordinates : TEXCOORD;
};

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);

float4 main(DomainOutput input) : SV_TARGET
{
    // Compute diffuse albedo by multiplying the sample from the texture and the diffuse albedo of the material:
    float4 diffuseAlbedo = DiffuseMap.Sample(SamplerAnisotropicClamp, input.TextureCoordinates) * DiffuseAlbedo;

    // TODO implment normal mapping
    float3 normalW = float3(0.0f, 1.0f, 0.0f);

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
    float4 lightIntensity = ComputeLighting(Lights, material, input.PositionW, normalW, toEyeDirection);
    
    // The final color results from the sum of the indirect and direct light:
    float4 color = ambientIntensity + lightIntensity;

    // Common convention to take alpha from diffuse albedo:
    color.a = diffuseAlbedo.a;

    return color;
}
