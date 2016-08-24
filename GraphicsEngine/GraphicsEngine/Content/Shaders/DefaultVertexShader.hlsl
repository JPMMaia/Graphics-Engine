#include "LightingUtils.hlsl"
#include "ObjectConstants.hlsl"
#include "PassConstants.hlsl"
#include "PassConstants.hlsl"

struct VertexIn
{
	float3 PositionL : POSITION;
	float3 NormalL : NORMAL;
};

struct VertexOut
{
	float4 PositionH : SV_POSITION;
	float3 PositionW : POSITION;
	float3 NormalW : NORMAL;
};

ConstantBuffer<ObjectConstants> g_objectCB : register(b0);
ConstantBuffer<PassConstants> g_passCB : register(b2);

VertexOut main(VertexIn input)
{
	VertexOut output;

	// Transfrom to World space:
	float4 positionW = mul(float4(input.PositionL, 1.0f), g_objectCB.WorldMatrix);
	output.PositionW = positionW.xyz;

	// Assumes nonuniform scaling:
	output.NormalW = mul(input.NormalL, (float3x3) g_objectCB.WorldMatrix);

	// Transform to Homogeneous clip space:
	output.PositionH = mul(positionW, g_passCB.ViewProjectionMatrix);

	return output;
}
