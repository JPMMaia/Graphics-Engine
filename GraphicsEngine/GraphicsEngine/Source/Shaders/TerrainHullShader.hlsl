#define NUM_CONTROL_POINTS 4

cbuffer CameraConstantBuffer : register(b0)
{
	float4x4 g_viewProjectionMatrix;
	float3 g_eyePositionW;
};

cbuffer TesselationConstantBuffer : register(b1)
{
	float g_maxTesselationDistance;
	float g_minTesselationDistance;
	float g_maxTesselationFactor;
	float g_minTesselationFactor;
}

struct VertexOutput
{
	float3 PositionW : POSITION;
	float2 TextureCoordinate : TEXCOORD0;
};

struct HullOutput
{
	float3 PositionW : POSITION;
	float2 TextureCoordinate : TEXCOORD0;
};

struct TesselationPatch
{
	float EdgeTesselationFactor[4] : SV_TessFactor;
	float InsideTesselationFactor[2] : SV_InsideTessFactor;
};

float CalculateTesselationFactor(float3 positionW)
{
	// Calculate the distance from the vertex to the camera:
	float distanceToEye = distance(g_eyePositionW, positionW);

	// Calculate the tesselation factor:
	float tesselationDistanceScalar = saturate((g_minTesselationDistance - distanceToEye) / (g_minTesselationDistance - g_maxTesselationDistance));
	return pow(2, lerp(g_minTesselationFactor, g_maxTesselationFactor, tesselationDistanceScalar));
}

TesselationPatch CalcHSPatchConstants(InputPatch<VertexOutput, NUM_CONTROL_POINTS> inputPatch, uint patchID : SV_PrimitiveID)
{
	TesselationPatch output;

	// Calculate the center of each edge:
	// 1st u==0, 2nd v==0, 3rd u==1, 4th v==1
	// The ordering of the edges is clockwise, starting from the u==0, which is the left side of the patch, and from the v==0 edge, which is the top of the patch:
	float3 leftEdgeCenter = 0.5f * (inputPatch[0].PositionW + inputPatch[2].PositionW);
	float3 topEdgeCenter = 0.5f * (inputPatch[2].PositionW + inputPatch[3].PositionW);
	float3 rightEdgeCenter = 0.5f * (inputPatch[1].PositionW + inputPatch[3].PositionW);
	float3 bottomEdgeCenter = 0.5f * (inputPatch[0].PositionW + inputPatch[1].PositionW);

	// Calculate the tesselation factor of each edge:
	output.EdgeTesselationFactor[0] = CalculateTesselationFactor(leftEdgeCenter);
	output.EdgeTesselationFactor[1] = CalculateTesselationFactor(topEdgeCenter);
	output.EdgeTesselationFactor[2] = CalculateTesselationFactor(rightEdgeCenter);
	output.EdgeTesselationFactor[3] = CalculateTesselationFactor(bottomEdgeCenter);

	// Calculate the inside tesselation factors:
	float3 center = 0.25f * (inputPatch[0].PositionW + inputPatch[1].PositionW + inputPatch[2].PositionW + inputPatch[3].PositionW);
	output.InsideTesselationFactor[0] = CalculateTesselationFactor(center);
	output.InsideTesselationFactor[1] = output.InsideTesselationFactor[0];

	return output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
HullOutput main(InputPatch<VertexOutput, NUM_CONTROL_POINTS> inputPatch, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
	HullOutput output;

	output.PositionW = inputPatch[i].PositionW;
	output.TextureCoordinate = inputPatch[i].TextureCoordinate;

	return output;
}