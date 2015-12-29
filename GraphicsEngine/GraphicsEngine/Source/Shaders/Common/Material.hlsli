#ifndef _MATERIAL_
#define _MATERIAL_

struct Material
{
	float4 Ambient;
	float4 Diffuse;
	float4 Specular;
	float Shininess;
	float3 Pad;
};

#endif