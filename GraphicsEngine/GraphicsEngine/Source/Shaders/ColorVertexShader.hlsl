cbuffer CameraConstantBuffer : register(b0)
{
	float4x4 g_viewProjectionMatrix;
	float3 g_eyePositionW;
};

struct VertexInput
{
	float3 PositionL : POSITION;
	float4 Color : COLOR;
	row_major float4x4 WorldMatrix : WORLD;
};

struct VertexOutput
{
	float4 PositionH : SV_POSITION;
	float4 Color : COLOR;
};

VertexOutput main(VertexInput input)
{
	VertexOutput output;

	float4 worldPosition = mul(float4(input.PositionL, 1.0f), input.WorldMatrix);
	output.PositionH = mul(worldPosition, g_viewProjectionMatrix);
	output.Color = input.Color;

	return output;
}