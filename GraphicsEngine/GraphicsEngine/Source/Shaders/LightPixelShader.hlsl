#include <Common/Material.hlsli>
#include <Common/DirectionalLight.hlsli>
#include <Common/PointLight.hlsli>
#include <Common/SpotLight.hlsli>
#include <Common/NormalMapHelper.hlsli>

cbuffer PerSubsetConstantBuffer : register(b1)
{
	Material g_material;
};

cbuffer PerFrameConstantBuffer : register(b2)
{
	DirectionalLight g_directionalLight;
	PointLight g_pointLight;
	SpotLight g_spotLight;
	float3 g_EyePositionW;
};

struct VertexOutput
{
	float4 PositionH : SV_POSITION;
	float3 PositionW : POSITION;
	float3 NormalW : NORMAL;
	float2 TextureCoordinate : TEXCOORD0;
};

#ifdef USE_TEXTURE
SamplerState g_samplerState : register(s0);
Texture2D g_texture : register(t0);
#endif

float4 main(VertexOutput input) : SV_TARGET
{
	// Normalize normal as it could have been unnormalized during the interpolation:
	input.NormalW = normalize(input.NormalW);

	// Calculate the eye vector:
	float3 toEyeW = normalize(g_EyePositionW - input.PositionW);

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 lightAmbient;
	float4 lightDiffuse;
	float4 lightSpecular;

	// Compute directional light:
	ComputeDirectionalLight(g_directionalLight, g_material, input.NormalW, toEyeW, lightAmbient, lightDiffuse, lightSpecular);
	ambient += lightAmbient;
	diffuse += lightDiffuse;
	specular += lightSpecular;

	// Compute point light:
	ComputePointLight(g_pointLight, g_material, input.PositionW, input.NormalW, toEyeW, lightAmbient, lightDiffuse, lightSpecular);
	ambient += lightAmbient;
	diffuse += lightDiffuse;
	specular += lightSpecular;

	// Comput spot light:
	ComputeSpotLight(g_spotLight, g_material, input.PositionW, input.NormalW, toEyeW, lightAmbient, lightDiffuse, lightSpecular);
	ambient += lightAmbient;
	diffuse += lightDiffuse;
	specular += lightSpecular;

	
#ifdef USE_TEXTURE

	// Sample texture:
	float4 textureColor = g_texture.Sample(g_samplerState, input.TextureCoordinate);

	// Add all terms together:
	float4 color = textureColor * (ambient + diffuse) + specular;

	// Calculate alpha using the alpha channel from both the texture and the diffuse material:
	color.a = textureColor.a * g_material.Diffuse.a;

#else
	
	// Add all terms together:
	float4 color = ambient + diffuse + specular;

	// Use the alpha from the diffuse material:
	color.a = g_material.Diffuse.a;

#endif

	return color;
}