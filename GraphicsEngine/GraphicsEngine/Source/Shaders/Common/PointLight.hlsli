#ifndef _POINT_LIGHT_
#define _POINT_LIGHT_

#include "Material.hlsli"

struct PointLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Position;
	float Range;
	float3 Attenuation;
	float Pad;
};

void ComputePointLight(PointLight light, Material material, float3 position, float3 normal, float3 toEyeVector, out float4 ambient, out float4 diffuse, out float4 specular)
{
	// Initialize output variables to zero:
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// Calculate the light vector:
	float3 toLightVector = light.Position - position;

	// Calculate the distance to the light:
	float distance = length(toLightVector);
	if (distance > light.Range)
		return;

	// Normalize the light vector:
	toLightVector /= distance;

	// Calculate ambient term:
	ambient = material.Ambient * light.Ambient;

	// Calculate the diffuse intensity:
	float diffuseIntensity = dot(toLightVector, normal);

	if (diffuseIntensity > 0.0f)
	{
		// Calculate the diffuse term:
		diffuse = material.Diffuse * diffuseIntensity * light.Diffuse;

		// Calculate the reflection vector:
		float3 reflectionVector = reflect(-toLightVector, normal);

		// Calculate the specular term:
		float specularIntensity = pow(max(dot(reflectionVector, toEyeVector), 0.0f), material.Shininess);
		specular = material.Specular * specularIntensity * light.Specular;
	}

	// Calculate the attenuation factor:
	float attenuation = 1.0f / dot(light.Attenuation, float3(1.0f, distance, distance*distance));
	diffuse *= attenuation;
	specular *= attenuation;
}

#endif