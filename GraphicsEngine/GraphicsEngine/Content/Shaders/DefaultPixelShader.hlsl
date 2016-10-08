#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

#ifndef NUM_TEXTURES
#define NUM_TEXTURES 4
#endif

#include "LightingUtils.hlsl"
#include "MaterialData.hlsl"
#include "PassData.hlsl"

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
	nointerpolation uint MaterialIndex : MATERIALINDEX;
};

ConstantBuffer<PassData> g_passData : register(b0);

Texture2D g_diffuseMap[NUM_TEXTURES] : register(t0, space0);
StructuredBuffer<MaterialData> g_materialData : register(t1, space1);

float4 main(VertexOut input) : SV_TARGET
{
	// Fetch material data:
	MaterialData materialData = g_materialData[input.MaterialIndex];

	// Calculate diffuse albedo by multiplying the sampling value of the diffuse map by the material diffuse albedo:
	float4 diffuseAlbedo = g_diffuseMap[materialData.DiffuseMapIndex].Sample(g_samplerAnisotropicWrap, input.TextureCoordinates) * materialData.DiffuseAlbedo;

#if defined(ALPHA_TEST)
	
	// Clip if alpha is near 0:
	clip(diffuseAlbedo.a - 0.1f);

#endif

	// Interpolating normal can unormalize it, so normalize it:
	input.NormalW = normalize(input.NormalW);

	// Vector from point being lit to eye:
	float3 toEyeW = g_passData.EyePositionW - input.PositionW;
	float distanceToEyeW = length(toEyeW);
	toEyeW /= distanceToEyeW;

	// Indirect lighting:
	float4 ambientLight = g_passData.AmbientLight * materialData.DiffuseAlbedo;

	// Direct lighting:
	const float shininess = 1.0f - materialData.Roughness;
	Material material = { diffuseAlbedo, materialData.FresnelR0, shininess };
	float3 shadowFactor = float3(1.0f, 1.0f, 1.0f);
	float4 directLight = ComputeLighting(g_passData.Lights, material, input.PositionW, input.NormalW, toEyeW, shadowFactor);

	float4 litColor = ambientLight + directLight;

#if defined(FOG)

	float fogIntensity = saturate((distanceToEyeW - g_passData.FogStart) / g_passData.FogRange);
	litColor = (1.0f - fogIntensity) * litColor + fogIntensity * g_passData.FogColor;

#endif

	// Common convention to take alpha from diffuse albedo:
	litColor.a = diffuseAlbedo.a;

	return litColor;
}
