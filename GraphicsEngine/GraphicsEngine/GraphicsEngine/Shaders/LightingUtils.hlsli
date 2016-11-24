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
float3 ShlickFresnel(float3 r0, float3 normal, float3 lightDirection)
{
    float cosIncidentAngle = saturate(dot(normal, lightDirection));
    float f0 = 1.0f - cosIncidentAngle;

    return r0 + (1.0f - r0) * (f0 * f0 * f0 * f0 * f0);
}

/// <sumary>
/// Evaluates the light strength given the parameters.
/// </sumary>
float3 BlinnPhong(float3 lightStrength, float3 lightDirection, float3 normal, float3 toEyeDirection, Material material)
{
    const float m = material.Shininess * 256.0f;
    float3 halfVector = normalize(toEyeDirection + lightDirection);

    float roughnessFactor = (m + 8.0f) * pow(max(dot(halfVector, normal), 0.0f), m) / 8.0f;


    // TODO Change HalfVector to Normal and spot differences
    float3 fresnelFactor = ShlickFresnel(material.FresnelR0, halfVector, lightDirection);

    float3 specularAlbedo = fresnelFactor * roughnessFactor;

    // TODO Spot differences after commenting this line
    specularAlbedo = specularAlbedo / (specularAlbedo + 1.0f);

    return (material.DiffuseAlbedo.rgb + specularAlbedo) * lightStrength;
}

/// <sumary>
/// Evaluates the lighting equation for directional lights.
/// </sumary>
float3 ComputeDirectionalLight(Light light, Material material, float3 normal, float3 toEyeDirection)
{
    float3 lightDirection = -light.Direction;

    // Calculate Light's strenght using the Lambert's Cosine Law
    float3 lightStrength = light.Strength * max(dot(lightDirection, normal), 0.0f);

    return BlinnPhong(lightStrength, lightDirection, normal, toEyeDirection, material);
}

/// <sumary>
/// Evaluates the lighting equation for point lights.
/// </sumary>
float3 ComputePointLight(Light light, Material material, float3 position, float3 normal, float3 toEyeDirection)
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

    return BlinnPhong(lightStrength, lightDirection, normal, toEyeDirection, material);
}

/// <sumary>
/// Evaluates the lighting equation for spot lights.
/// </sumary>
float3 ComputeSpotLight(Light light, Material material, float3 position, float3 normal, float3 toEyeDirection)
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

    return BlinnPhong(lightStrength, lightDirection, normal, toEyeDirection, material);

}

/// <sumary>
/// Evaluates the accumulated light intensity of all lights.
/// </sumary>
float4 ComputeLighting(Light lights[MAX_NUM_LIGHTS], Material material, float3 position, float3 normal, float3 toEyeDirection)
{
    float3 result = 0.0f;

#if (NUM_DIR_LIGHTS > 0)
    
    [unroll]
    for (int i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        result += ComputeDirectionalLight(lights[i], material, normal, toEyeDirection);
    }

#endif 
	
#if (NUM_POINT_LIGHTS > 0)

    [unroll]
    for (i = NUM_DIR_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; ++i)
    {
        result += ComputePointLight(lights[i], material, position, normal, toEyeDirection);
    }

	#endif

#if (NUM_SPOT_LIGHTS > 0)
	
    [unroll]
    for (i = NUM_DIR_LIGHTS + NUM_POINT_LIGHTS; i < NUM_DIR_LIGHTS + NUM_POINT_LIGHTS + NUM_SPOT_LIGHTS; ++i)
    {
        result += ComputeSpotLight(lights[i], material, position, normal, toEyeDirection);
    }

#endif

    return float4(result, 0.0f);
}
