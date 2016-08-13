struct ObjectConstants
{
	float4x4 WorldMatrix;
};

struct PassConstants
{
	float4x4 ViewMatrix;
	float4x4 InverseViewMatrix;
	float4x4 ProjectionMatrix;
	float4x4 InverseProjectionMatrix;
	float4x4 ViewProjectionMatrix;
	float4x4 InverseViewProjectionMatrix;
	float3 EyePositionW;
	float Padding;
	float2 RenderTargetSize;
	float2 InverseRenderTargetSize;
	float NearZ;
	float FarZ;
	float TotalTime;
	float DeltaTime;
};

ConstantBuffer<ObjectConstants> g_objectCB : register(b0);
ConstantBuffer<PassConstants> g_passCB : register(b1);

struct VertexIn
{
	float3 PositionL : POSITION;
	float4 Color : COLOR;
};

struct VertexOut
{
	float4 PositionH : SV_POSITION;
	float4 Color : COLOR;
};

VertexOut main(VertexIn input)
{
	VertexOut output;

	float4 positionW = mul(float4(input.PositionL, 1.0f), g_objectCB.WorldMatrix);
	output.PositionH = mul(positionW, g_passCB.ViewProjectionMatrix);
	output.Color = input.Color;

	return output;
}