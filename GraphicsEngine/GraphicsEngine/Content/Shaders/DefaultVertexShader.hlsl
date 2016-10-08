#include "LightingUtils.hlsl"
#include "ObjectData.hlsl"
#include "MaterialData.hlsl"
#include "PassData.hlsl"

struct VertexIn
{
	float3 PositionL : POSITION;
	float3 NormalL : NORMAL;
	float2 TextureCoordinates : TEXCOORD0;
};

struct VertexOut
{
	float4 PositionH : SV_POSITION;
	float3 PositionW : POSITION;
	float3 NormalW : NORMAL;
	float2 TextureCoordinates : TEXCOORD0;
};

ConstantBuffer<ObjectData> g_objectData : register(b0);
ConstantBuffer<PassData> g_passData : register(b1);

StructuredBuffer<MaterialData> g_materialData : register(t0, space1);

VertexOut main(VertexIn input)
{
	VertexOut output;

	// Transfrom to World space:
	float4 positionW = mul(float4(input.PositionL, 1.0f), g_objectData.WorldMatrix);
	output.PositionW = positionW.xyz;

	// Assumes nonuniform scaling:
	output.NormalW = mul(input.NormalL, (float3x3) g_objectData.WorldMatrix);

	// Transform to Homogeneous clip space:
	output.PositionH = mul(positionW, g_passData.ViewProjectionMatrix);

	// Fetch material data and calculate texture coordinates:
	MaterialData materialData = g_materialData[g_objectData.MaterialIndex];
	float4 textureCoordinates = mul(float4(input.TextureCoordinates, 0.0f, 1.0f), g_objectData.TextureTransform);
	output.TextureCoordinates = mul(textureCoordinates, materialData.MaterialTransform).xy;

	return output;
}
