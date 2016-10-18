#include <Common/Material.hlsli>
#include <Common/DirectionalLight.hlsli>
#include <Common/PointLight.hlsli>
#include <Common/SpotLight.hlsli>

cbuffer CameraConstantBuffer : register(b0)
{
	float4x4 g_viewProjectionMatrix;
	float3 g_eyePositionW;
};

cbuffer SubsetConstantBuffer : register(b2)
{
	Material g_material;
};

cbuffer FrameConstantBuffer : register(b3)
{
	DirectionalLight g_directionalLight;
	PointLight g_pointLight;
	SpotLight g_spotLight;
};

struct DomainOutput
{
	float4 PositionH : SV_POSITION;
	float3 PositionW : POSITION;
	float2 TextureCoordinate : TEXCOORD0;
};

SamplerState g_samplerState : register(s0);
Texture2D g_texture : register(t0);

float4 main(DomainOutput input) : SV_TARGET
{
	/*// Calculate the eye vector:
	float3 toEyeW = normalize(g_eyePositionW - input.PositionW);

	// Normalize normal as it could have been unnormalized during the interpolation:
	float3 unitNormalW = normalize(input.NormalW);

	float4 ambient = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 diffuse = float4(0.0f, 0.0f, 0.0f, 0.0f);
	float4 specular = float4(0.0f, 0.0f, 0.0f, 0.0f);

	float4 lightAmbient;
	float4 lightDiffuse;
	float4 lightSpecular;

	// Compute directional light:
	ComputeDirectionalLight(g_directionalLight, g_material, unitNormalW, toEyeW, lightAmbient, lightDiffuse, lightSpecular);
	ambient += lightAmbient;
	diffuse += lightDiffuse;
	specular += lightSpecular;

	// Compute point light:
	ComputePointLight(g_pointLight, g_material, input.PositionW, unitNormalW, toEyeW, lightAmbient, lightDiffuse, lightSpecular);
	ambient += lightAmbient;
	diffuse += lightDiffuse;
	specular += lightSpecular;

	// Comput spot light:
	ComputeSpotLight(g_spotLight, g_material, input.PositionW, unitNormalW, toEyeW, lightAmbient, lightDiffuse, lightSpecular);
	ambient += lightAmbient;
	diffuse += lightDiffuse;
	specular += lightSpecular;

	// Sample texture:
	//float4 textureColor = g_texture.Sample(g_samplerState, input.TextureCoordinate);
	float4 textureColor = float4(1.0f, 1.0f, 1.0f, 1.0f);

	// Add all terms together:
	float4 color = textureColor * (ambient + diffuse) + specular;

	// Calculate alpha using the alpha channel from both the texture and the diffuse material:
	color.a = textureColor.a * g_material.Diffuse.a;

	return color;*/

	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}