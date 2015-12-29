#ifndef _SPOT_LIGHT_
#define _SPOT_LIGHT_

struct SpotLight
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float3 Position;
	float Range;
	float3 Direction;
	float Spot;
	float3 Attenuation;
	float Pad;
};

void ComputeSpotLight(SpotLight light, Material material, float3 position, float3 normal, float3 toEyeVector, out float4 ambient, out float4 diffuse, out float4 specular)
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

	// Apply the light contribution:
	ApplyLight(light.Ambient, light.Diffuse, light.Specular, material, normal, toEyeVector, toLightVector, ambient, diffuse, specular);

	// Apply the attenuation factor:
	ApplyAttenuation(light.Attenuation, distance, diffuse, specular);

	// Apply the spot intensity:
	float spotIntensity = pow(max(dot(-toLightVector, light.Direction), 0.0f), light.Spot);
	ambient *= spotIntensity;
	diffuse *= spotIntensity;
	specular *= spotIntensity;
}

#endif