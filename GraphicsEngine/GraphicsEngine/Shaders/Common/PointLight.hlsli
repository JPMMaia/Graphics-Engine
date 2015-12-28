#ifndef _POINT_LIGHT_
#define _POINT_LIGHT_

#include "Material.hlsli"

struct PointLight
{
	float4 Ambient;
	float4 Diffuse;
	float3 Specular;
	float Shininess;
	float3 Position;
	float Range;
	float3 Attenuation;
	float Pad;
};

#endif