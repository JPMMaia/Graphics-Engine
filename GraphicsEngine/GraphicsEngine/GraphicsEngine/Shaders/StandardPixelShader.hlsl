#include "LightingUtils.hlsli"
#include "MaterialData.hlsli"
#include "PassData.hlsli"
#include "Samplers.hlsli"
#include "MathUtils.hlsli"

struct VertexOutput
{
    float3 PositionW : POSITION0;
    float4 PositionH : SV_POSITION;
    float4 ShadowPositionH : POSITION1;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TextureCoordinates : TEXCOORD;
};

Texture2D DiffuseMap : register(t0);
Texture2D NormalMap : register(t1);
Texture2D SpecularMap : register(t2);
Texture2D ShadowMap : register(t3);

float4 main(VertexOutput input) : SV_TARGET
{
    // Compute diffuse albedo by multiplying the sample from the texture and the diffuse albedo of the material:
    float4 diffuseAlbedo = DiffuseMap.Sample(SamplerAnisotropicWrap, input.TextureCoordinates) * DiffuseAlbedo;

#if defined(ALPHA_CLIPPING)
	
	// Clip if alpha is near 0:
	clip(diffuseAlbedo.a - 0.1f);

#endif

    // Interpolating rasterization process can change the magnitude of the normal vector:
    float3 normalW = normalize(input.NormalW);
    float3 tangentW = normalize(input.TangentW);

#if defined(NORMAL_MAPPING)

    // Sample value from the normal map and compute the bumped normal in world space:
    float3 normalSample = NormalMap.Sample(SamplerAnisotropicWrap, input.TextureCoordinates).rgb;
    normalW = NormalSampleToBumpedNormalW(normalSample, normalW, tangentW);

#endif

    float specularFactor;

#if defined(SPECULAR_MAPPING)

    specularFactor = SpecularMap.Sample(SamplerAnisotropicWrap, input.TextureCoordinates).r;

#else

    specularFactor = 1.0f;

#endif

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
    //float shadowFactor = CalculateShadowFactor(ShadowMap, SamplerShadows, input.ShadowPositionH);
    float shadowFactor = 1.0f;

    // Compute contribution of lights:
    float4 lightIntensity = ComputeLighting(Lights, material, input.PositionW, normalW, toEyeDirection, shadowFactor, specularFactor);
    
    // The final color results from the sum of the indirect and direct light:
    float4 color = ambientIntensity + lightIntensity;

#if defined(FOG)
    color = AddFog(color, distanceToEye, FogStart, FogRange, FogColor);
#endif

    // Common convention to take alpha from diffuse albedo:
    color.a = diffuseAlbedo.a;

    return color;
}
