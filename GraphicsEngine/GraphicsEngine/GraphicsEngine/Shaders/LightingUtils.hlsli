#ifndef MAX_NUM_LIGHTS
#define MAX_NUM_LIGHTS 16
#endif

#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

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

float3 ComputeDirectionalLight(Light light, Material material, float3 normal, float3 toEyeDirection)
{
    float3 lightDirection = -light.Direction;

    // Calculate Light's strenght using the Lambert's Cosine Law
    float3 lightStrength = light.Strength * max(dot(lightDirection, normal), 0.0f);

    return BlinnPhong(lightStrength, lightDirection, normal, toEyeDirection, material);
}

float4 ComputeLighting(Light lights[MAX_NUM_LIGHTS], Material material, float3 position, float3 normal, float3 toEyeDirection)
{
    float3 result = 0.0f;

#if (NUM_DIR_LIGHTS > 0)
    
    for (int i = 0; i < NUM_DIR_LIGHTS; ++i)
    {
        result += ComputeDirectionalLight(lights[i], material, normal, toEyeDirection);
    }    

#endif 

    return float4(result, 0.0f);
}
