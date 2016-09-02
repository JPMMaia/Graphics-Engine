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

SamplerState g_samplerPointWrap : register(s0);
SamplerState g_samplerPointClamp : register(s1);
SamplerState g_samplerLinearWrap : register(s2);
SamplerState g_samplerLinearClamp : register(s3);
SamplerState g_samplerAnisotropicWrap : register(s4);
SamplerState g_samplerAnisotropicClamp : register(s5);

struct VertexOut
{
	float4 PositionH : SV_POSITION;
	float3 PositionW : POSITION;
	float3 NormalW : NORMAL;
	float2 TextureCoordinates : TEXCOORD0;
};

ConstantBuffer<MaterialConstants> g_materialCB : register(b1);
ConstantBuffer<PassConstants> g_passCB : register(b2);

Texture2D g_diffuseMap : register(t0);

float4 main(VertexOut input) : SV_TARGET
{

#if defined(ALPHA_TEST)
	
	// Clip if alpha is near 0:
	clip(g_materialCB.DiffuseAlbedo.a - 0.1f);

#endif

	// Interpolating normal can unormalize it, so normalize it:
	input.NormalW = normalize(input.NormalW);

	// Vector from point being lit to eye:
	float3 toEyeW = g_passCB.EyePositionW - input.PositionW;
	float distanceToEyeW = length(toEyeW);
	toEyeW /= distanceToEyeW;

	// Indirect lighting:
	float4 ambientLight = g_passCB.AmbientLight * g_materialCB.DiffuseAlbedo;

	// Direct lighting:
	const float shininess = 1.0f - g_materialCB.Roughness;
	float4 diffuseAlbedo = g_diffuseMap.Sample(g_samplerAnisotropicWrap, input.TextureCoordinates) * g_materialCB.DiffuseAlbedo;
	Material material = { diffuseAlbedo, g_materialCB.FresnelR0, shininess };
	float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
	float4 directLight = ComputeLighting(g_passCB.Lights, material, input.PositionW, input.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambientLight + directLight;

#if defined(FOG)

	float fogIntensity = saturate((distanceToEyeW - g_passCB.FogStart) / g_passCB.FogRange);
	litColor = (1.0f - fogIntensity) * litColor + fogIntensity * g_passCB.FogColor;

#endif

	// Common convention to take alpha from diffuse material:
	litColor.a = g_materialCB.DiffuseAlbedo.a;

	return litColor;
}
