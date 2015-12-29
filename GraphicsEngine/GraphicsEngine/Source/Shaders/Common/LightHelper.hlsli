#ifndef _LIGHT_HELPER_
#define _LIGHT_HELPER_

#include "Material.hlsli"

void ApplyLight(float4 lightAmbient, float4 lightDiffuse, float4 lightSpecular, Material material, float3 normal, float3 toEyeVector, float3 toLightVector, inout float4 ambient, inout float4 diffuse, inout float4 specular)
{
	// Calculate the ambient term:
	ambient += material.Ambient * lightAmbient;

	// Calculate diffuse intensity:
	float diffuseIntensity = dot(toLightVector, normal);

	if (diffuseIntensity > 0.0f)
	{
		// Calculate the diffuse term:
		diffuse += material.Diffuse * diffuseIntensity * lightDiffuse;

		// Calculate the reflection vector:
		float3 reflectionVector = reflect(-toLightVector, normal);

		// Calculate the specular term:
		float specularIntensity = pow(max(dot(reflectionVector, toEyeVector), 0.0f), material.Shininess);
		specular += material.Specular * specularIntensity * lightSpecular;
	}
}

void ApplyAttenuation(float3 attenuationCoefficients, float distance, inout float4 diffuse, inout float4 specular)
{
	float attenuation = 1.0f / dot(attenuationCoefficients, float3(1.0f, distance, distance*distance));
	diffuse *= attenuation;
	specular *= attenuation;
}

#endif