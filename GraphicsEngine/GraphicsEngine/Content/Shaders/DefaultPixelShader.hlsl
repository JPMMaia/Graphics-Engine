#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

#include "LightingUtils.hlsl"
#include "PassConstants.hlsl"
#include "MaterialConstants.hlsl"

struct VertexOut
{
	float4 PositionH : SV_POSITION;
	float3 PositionW : POSITION;
	float3 NormalW : NORMAL;
};

ConstantBuffer<MaterialConstants> g_materialCB : register(b1);
ConstantBuffer<PassConstants> g_passCB : register(b2);

float4 main(VertexOut input) : SV_TARGET
{
	// Interpolating normal can unormalize it, so normalize it:
	input.NormalW = normalize(input.NormalW);

	// Vector from point being lit to eye:
	float3 toEyeW = normalize(g_passCB.EyePositionW - input.PositionW);

	// Indirect lighting:
	float4 ambientLight = g_passCB.AmbientLight * g_materialCB.DiffuseAlbedo;

	// Direct lighting:
	const float shininess = 1.0f - g_materialCB.Roughness;
	Material material = { g_materialCB.DiffuseAlbedo, g_materialCB.FresnelR0, shininess };
	float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
	float4 directLight = ComputeLighting(g_passCB.Lights, material, input.PositionW, input.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambientLight + directLight;

	// Common convention to take alpha from diffuse material:
	litColor.a = g_materialCB.DiffuseAlbedo.a;

	return litColor;
}
