#include "LightingUtils.hlsl"
#include "InstanceData.hlsl"
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
	nointerpolation uint MaterialIndex : MATERIALINDEX;
};

ConstantBuffer<PassData> g_passData : register(b0);

StructuredBuffer<InstanceData> g_instanceData : register(t0, space1);
StructuredBuffer<MaterialData> g_materialData : register(t1, space1);

VertexOut main(VertexIn input, uint instanceID : SV_InstanceID)
{
	VertexOut output;

	// Fetch instance and material data:
	InstanceData instanceData = g_instanceData[instanceID];
	MaterialData materialData = g_materialData[instanceData.MaterialIndex];

	// Transfrom to World space:
	float4 positionW = mul(float4(input.PositionL, 1.0f), instanceData.WorldMatrix);
	output.PositionW = positionW.xyz;

	// Assumes nonuniform scaling:
	output.NormalW = mul(input.NormalL, (float3x3) instanceData.WorldMatrix);

	// Transform to Homogeneous clip space:
	output.PositionH = mul(positionW, g_passData.ViewProjectionMatrix);

	// Calculate texture coordinates:
	float4 textureCoordinates = mul(float4(input.TextureCoordinates, 0.0f, 1.0f), instanceData.TextureTransform);
	output.TextureCoordinates = mul(textureCoordinates, materialData.MaterialTransform).xy;

	// Set the material index:
	output.MaterialIndex = instanceData.MaterialIndex;

	return output;
}
