#ifndef _DIRECTIONAL_LIGHT_
#define _DIRECTIONAL_LIGHT_

#include "Material.hlsli"

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float3 Specular;
	float Shininess;
	float3 Direction;
	float Pad;
};

void ComputeDirectionalLight(DirectionalLight light, Material material, float3 normal, float3 toEyeVector, out float4 ambient, out float4 diffuse, out float3 specular)
{
	// Initialize output variables to zero:
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float3(0.0f, 0.0f, 0.0f);

	// The light vector has the opposite direction of the light direction:
	float3 toLightVector = -light.Direction;

	// Calculate the ambient term:
	ambient = material.Ambient * light.Ambient;

	// Calculate diffuse intensity:
	float diffuseIntensity = dot(toLightVector, normal);

	if (diffuseIntensity > 0.0f)
	{
		// Calculate the diffuse term:
		diffuse = diffuseIntensity * material.Diffuse * light.Diffuse;

		// Calculate the reflection vector:
		float3 reflectionVector = reflect(-toLightVector, normal);

		// Calculate the specular term:
		float specularIntensity = pow(max(dot(reflectionVector, toEyeVector), 0.0f), material.Shininess);
		specular = specularIntensity * material.Specular, 1.0f * light.Specular;
	}
}

#endif