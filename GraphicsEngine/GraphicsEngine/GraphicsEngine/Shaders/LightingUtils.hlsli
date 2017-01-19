#include "CommonDefines.hlsli"

struct Light
{
    float3 Strength; // Light color
    float FalloffStart; // Point/Spot light only
    float3 Direction; // Directional/Spot light only
    float FalloffEnd; // Point/Spot light only
    float3 Position; // Point/Spot light only
    float SpotPower; // Spot light only
};

struct Material
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Shininess;
};

float CalculateShadowFactor(Texture2D shadowMap, SamplerComparisonState samplerState, float4 shadowPositionH)
{
    // Perform perspective divide (if perspective projection, w = 1.0f for orthographic):
    shadowPositionH = shadowPositionH / shadowPositionH.w;

    float depth = shadowPositionH.z;

    // Get the dimensions of the shadow map:
    uint width, height, numberOfLevels;
    shadowMap.GetDimensions(0, width, height, numberOfLevels);

    // Calculate the size of a texel:
    float dx = 1.0f / (float) width;

    // Offsets for neighbors:
    const uint numberOfOffsets = 9;
    const float2 offsets[numberOfOffsets] =
    {
        float2(-dx, -dx), float2(0.0f, -dx), float2(+dx, -dx),
        float2(-dx, 0.0), float2(0.0f, 0.0f), float2(+dx, 0.0f),
        float2(-dx, +dx), float2(0.0f, +dx), float2(+dx, +dx),
    };

    float percentLit = 0.0f;
    [unroll]
    for (uint i = 0; i < numberOfOffsets; ++i)
    {
        // Sample texture from [position.x + dx, position.y + dy] and compare it againts the depth value:
        percentLit += shadowMap.SampleCmpLevelZero(samplerState, shadowPositionH.xy + offsets[i], depth).r;
    }

    return percentLit /= (float) numberOfOffsets;
}

/// <sumary>
/// Calculates the linear attenuation which applies to point and spot lights.
/// </sumary>
float CalculateAttenuation(float distance, float falloffStart, float falloffEnd)
{
    return saturate((falloffEnd - distance) / (falloffEnd - falloffStart));
}

/// <sumary>
/// Schlick gives an approximation to the Fresnel reflectance effect.
/// R(0) + (1 - R(0)) * (1 - cos(angle))^5
/// </sumary>
float3 SchlickFresnel(float3 r0, float3 normal, float3 lightDirection)
{
    float cosIncidentAngle = saturate(dot(normal, lightDirection));
    float f0 = 1.0f - cosIncidentAngle;

    return r0 + (1.0f - r0) * (f0 * f0 * f0 * f0 * f0);
}

/// <sumary>
/// Evaluates the light strength given the parameters.
/// </sumary>
float3 BlinnPhong(float3 lightStrength, float3 lightDirection, float3 normal, float3 toEyeDirection, Material material, float specularFactor)
{
    // Calculate m which is the shiness in the range [0, 256]:
    const float m = material.Shininess * 256.0f;

    // Calculate the half vector:
    float3 halfVector = normalize(toEyeDirection + lightDirection);

    // Calculate the roughness.
    // (m + 8) * (halfVector . normal)^m
    // ---------------------------------
    //               8
    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVector, normal), 0.0f), m) / 8.0f;

    // Calculate the fresnel factor using the Schlick approximation:
    float3 fresnelFactor = SchlickFresnel(material.FresnelR0, normal, lightDirection);

    // Calculate the specular albedo:
    float3 specularAlbedo = fresnelFactor * roughnessFactor;

    // Scale specular albedo a bit to reduce exessive brightness:
    specularAlbedo = specularAlbedo / (specularAlbedo + 1.0f);

    return (material.DiffuseAlbedo.rgb + specularAlbedo * specularFactor) * lightStrength;
}

/// <sumary>
/// Evaluates the lighting equation for directional lights.
/// </sumary>
float3 ComputeDirectionalLight(Light light, Material material, float3 normal, float3 toEyeDirection, float specularFactor)
{
    float3 lightDirection = -light.Direction;

    // Calculate Light's strenght using the Lambert's Cosine Law
    float3 lightStrength = light.Strength * max(dot(lightDirection, normal), 0.0f);

    return BlinnPhong(lightStrength, lightDirection, normal, toEyeDirection, material, specularFactor);
}

/// <sumary>
/// Evaluates the lighting equation for point lights.
/// </sumary>
float3 ComputePointLight(Light light, Material material, float3 position, float3 normal, float3 toEyeDirection, float specularFactor)
{
    float3 lightDirection = light.Position - position;

    float distance = length(lightDirection);
    if (distance > light.FalloffEnd)
        return float3(0.0f, 0.0f, 0.0f);

	// Normalize the vector
    lightDirection /= distance;

	// Scale light by Lambert's cosine law
    float ndotl = max(dot(lightDirection, normal), 0.0f);
    float3 lightStrength = light.Strength * ndotl;

	// Apply attenuation
    float attenuation = CalculateAttenuation(distance, light.FalloffStart, light.FalloffEnd);
    lightStrength *= attenuation;

    return BlinnPhong(lightStrength, lightDirection, normal, toEyeDirection, material, specularFactor);
}

/// <sumary>
/// Evaluates the lighting equation for spot lights.
/// </sumary>
float3 ComputeSpotLight(Light light, Material material, float3 position, float3 normal, float3 toEyeDirection, float specularFactor)
{
    float3 lightDirection = light.Position - position;

    float distance = length(lightDirection);
    if (distance > light.FalloffEnd)
        return float3(0.0f, 0.0f, 0.0f);

    lightDirection /= distance;
	
	// Scale light by Lambert's cosine law
    float3 ndotl = max(dot(lightDirection, normal), 0.0f);
    float3 lightStrength = light.Strength * ndotl;

	// Attenuate light
    float attenuation = CalculateAttenuation(distance, light.FalloffStart, light.FalloffEnd);
    lightStrength *= attenuation;

	// Scale by spot light
    float spotFactor = pow(max(dot(-lightDirection, light.Direction), 0.0f), light.SpotPower);
    lightStrength *= spotFactor;

    return BlinnPhong(lightStrength, lightDirection, normal, toEyeDirection, material, specularFactor);

}

/// <sumary>
/// Evaluates the accumulated light intensity of all lights.
/// </sumary>
float4 ComputeLighting(Light lights[MAX_NUM_LIGHTS], Material material, float3 position, float3 normal, float3 toEyeDirection, float shadowFactor, float specularFactor)
{
    float3 result = 0.0f;

#if (NUM_DIR_LIGHTS > 0)
    
    [unroll]
    for (int i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        result += shadowFactor * ComputeDirectionalLight(lights[i], material, normal, toEyeDirection, specularFactor);
    }

#endif 
	
#if (NUM_POINT_LIGHTS > 0)

    [unroll]
    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        result += ComputePointLight(lights[i], material, position, normal, toEyeDirection, specularFactor);
    }

	#endif

#if (NUM_SPOT_LIGHTS > 0)
	
    [unroll]
    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        result += ComputeSpotLight(lights[i], material, position, normal, toEyeDirection, specularFactor);
    }

#endif

    return float4(result, 0.0f);
}
