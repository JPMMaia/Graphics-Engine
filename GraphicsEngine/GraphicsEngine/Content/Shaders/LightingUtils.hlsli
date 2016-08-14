#pragma once

struct Light
{
	float3 Strength;		// Light color
	float FalloffStart;		// Point/Spot light only
	float3 Direction;		// Directional/Spot light only
	float FalloffEnd;		// Point/Spot light only
	float3 Position;		// Point/Spot light only
	float SpotPower;		// Spot light only
};

struct Material
{
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float Shininess;
};

/// <sumary>
///	Linear falloff.
/// </sumary>
float CalculateAttenuation(float distance, float falloffStart, float fallofEnd)
{
	return saturate((fallofEnd - distance) / (falloffEnd - falloffStart));
}

/// <sumary>
///	Schlick gives an approximation to Fresnel reflectance.
/// R0 = ( (n-1)/(n+1) )^2, where n is the index of refraction
/// </sumary>
float3 SchlickFresnel(float3 r0, float3 normal, float3 lightVector)
{
	float cosIncidentAngle = saturate(dot(normal, lightVector));
	float f0 = 1.0f - cosIncidentAngle;

	return r0 + (1.0f - r0) * (f0 * f0 * f0 * f0 * f0);
}

float3 BlinnPhong(float3 lightStrength, float3 lightVector, float3 normal, float3 toEye, Material material)
{
	// Derive m from the shininess, which is derived from the roughness:
	const float m = material.Shininess * 256.0f;

	// Calculate the halfway vector:
	float3 halfwayVector = normalize(toEye + lightVector);

	float roughnessFactor = (m + 8.0f) * pow(max(dot(halfwayVector, normal), 0.0f), m) / 8.0f;
	float3 fresnelFactor = SchlickFresnel(material.FresnelR0, halfwayVector, lightVector);

	// TODO specularAlbedo = specularAlbedo / (specularAlbedo + 1.0f);


}

float3 ComputeDirectionalLight(Light light, Material material, float3 normal, float3 toEye)
{
	// The light vector aims opposite the direction the light rays travel:
	float3 lightVector = -light.Direction;

	// Scale light down by Lambert's cosine law:
	float lightIntensity = max(dot(lightVector, normal), 0.0f);
	float3 lightStrength = lightIntensity * light.Strength;

	return BlinnPhong(lightStrength, lightVector, normal, toEye, material);
}

float3 ComputePointLight(Light light, Material material, float3 surfacePointPosition, float3 normal, float3 toEye)
{
	// The vector from the surface to the light:
	float3 lightVector = light.Position - surfacePointPosition;

	// The distance from surface to light:
	float distance = length(lightVector);

	// Range test:
	if (distance > light.FalloffEnd)
		return 0.0f;

	// Normalize the light vector:
	lightVector /= distance;

	// Scale light down by Lambert's cosine law:
	float lightIntensity = max(dot(lightVector, normal), 0.0f);
	float3 lightStrength = lightIntensity * light.Strength;

	// Attenuate light by distance:
	float attenuation = CalculateAttenuation(distance, light.FallOffStart, light.FalloffEnd);
	lightStrength *= attenuation;

	return BlinnPhong(lightStrength, lightVector, normal, toEye, material);
}

float3 ComputeSpotLight(Light light, Material material, float3 surfacePointPosition, float3 normal, float3 toEye)
{
	// The vector from the surface to the light:
	float3 lightVector = light.Position - surfacePointPosition;

	// The distance from surface to light:
	float distance = length(lightVector);

	// Range test:
	if (distance > light.FalloffEnd)
		return 0.0f;

	// Normalize the light vector:
	lightVector /= distance;

	// Scale light down by Lambert's cosine law:
	float lightIntensity = max(dot(lightVector, normal), 0.0f);
	float3 lightStrength = lightIntensity * light.Strength;

	// Attenuate light by distance:
	float attenuation = CalculateAttenuation(distance, light.FallOffStart, light.FalloffEnd);
	lightStrength *= attenuation;

	// Scale by spotlight:
	float spotFactor = pow(max(dot(-lightVector, light.Direction), 0.0f), light.SpotPower);
	lightStrength *= spotFactor;

	return BlinnPhong(lightStrength, lightVector, normal, toEye, material);
}
