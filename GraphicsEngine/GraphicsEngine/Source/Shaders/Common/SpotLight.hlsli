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

#endif