#include <Common/Material.hlsli>
#include <Common/DirectionalLight.hlsli>

cbuffer PerObjectConstantBuffer : register(b0)
{
	float4x4 g_worldMatrix;
	float4x4 g_worldInverseTransposeMatrix;
	float4x4 g_worldViewProjectionMatrix;
	Material g_material;
	float3 g_EyePositionW;
};

cbuffer PerFrameConstantBuffer : register(b1)
{
	DirectionalLight g_directionalLight;
};

struct VertexOutput
{
	float4 PositionH : SV_POSITION;
	float3 PositionW : POSITION;
	float3 NormalW : NORMAL;
};

float4 main(VertexOutput input) : SV_TARGET
{
	// Normalize normal as it could have been unnormalized during the interpolation:
	input.NormalW = normalize(input.NormalW);

	// Calculate the eye vector:
	float3 toEyeW = normalize(g_EyePositionW - input.PositionW);

	float4 ambient;
	float4 diffuse;
	float3 specular;

	// Compute directional light:
	ComputeDirectionalLight(g_directionalLight, g_material, input.NormalW, toEyeW, ambient, diffuse, specular);

	// Add all terms together and take the alpha channel from the material's diffuse term:
	float4 color = ambient + diffuse + float4(specular, 0.0f);
	color.a = g_material.Diffuse.a;

	return color;
}