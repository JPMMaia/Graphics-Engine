struct PerObjectCBType
{
	matrix WorldViewProjectionMatrix;
};

ConstantBuffer<PerObjectCBType> g_perObjectCB : register(b0);

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

	output.PositionH = mul(float4(input.PositionL, 1.0f), g_perObjectCB.WorldViewProjectionMatrix);
	output.Color = input.Color;

	return output;
}