#ifndef __MATERIAL_CONSTANTS__
#define __MATERIAL_CONSTANTS__

struct MaterialConstants
{
	float4 DiffuseAlbedo;
	float3 FresnelR0;
	float Roughness;
	float4x4 MaterialTransform;
};

#endif