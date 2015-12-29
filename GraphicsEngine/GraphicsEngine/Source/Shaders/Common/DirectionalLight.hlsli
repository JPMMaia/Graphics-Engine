#ifndef _DIRECTIONAL_LIGHT_
#define _DIRECTIONAL_LIGHT_

#include "LightHelper.hlsli"

struct DirectionalLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Direction;
	float Pad;
};

void ComputeDirectionalLight(DirectionalLight light, Material material, float3 normal, float3 toEyeVector, out float4 ambient, out float4 diffuse, out float4 specular)
{
	// Initialize output variables to zero:
	ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	// The light vector has the opposite direction of the light direction:
	float3 toLightVector = normalize(-light.Direction);

	// Apply the light contribution:
	ApplyLight(light.Ambient, light.Diffuse, light.Specular, material, normal, toEyeVector, toLightVector, ambient, diffuse, specular);
}

#endif