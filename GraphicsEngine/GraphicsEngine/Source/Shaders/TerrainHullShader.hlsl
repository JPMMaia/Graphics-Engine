#define NUM_CONTROL_POINTS 4
#define NUM_TESSELATION_FACTOR 4
#define NUM_INSIDE_TESSELATION_FACTOR 2

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
	float EdgeTesselationFactor[NUM_TESSELATION_FACTOR]				: SV_TessFactor;
	float InsideTesselationFactor[NUM_INSIDE_TESSELATION_FACTOR]	: SV_InsideTessFactor;
};

float CalculateTesselationFactor(float3 positionW)
{
	// Calculate the distance from the vertex to the camera:
	float distanceToEye = distance(g_eyePositionW, positionW);

	// Calculate tesselation factor
	float tesselationDistanceScalar = saturate((g_minTesselationDistance - distanceToEye) / (g_minTesselationDistance - g_maxTesselationDistance));
	return pow(2, lerp(g_minTesselationFactor, g_maxTesselationFactor, tesselationDistanceScalar));
}

TesselationPatch CalcHSPatchConstants(InputPatch<VertexOutput, NUM_CONTROL_POINTS> inputPatch, uint patchID : SV_PrimitiveID)
{
	TesselationPatch output;

	// Calculate the edge tesselation factors:
	[unroll]
	for (uint i = 0; i < NUM_CONTROL_POINTS; i++)
	{
		float3 edgeCenter = 0.5f * (inputPatch[i].PositionW + inputPatch[(i + 1) % NUM_CONTROL_POINTS].PositionW);
		output.EdgeTesselationFactor[i] = CalculateTesselationFactor(edgeCenter);
	}

	// Calculate the inside tesselation factors:
	float3 center = 0.25f * (inputPatch[0].PositionW + inputPatch[1].PositionW + inputPatch[2].PositionW + inputPatch[3].PositionW);
	output.InsideTesselationFactor[0] = CalculateTesselationFactor(center);

	[unroll]
	for (uint j = 1; j < NUM_INSIDE_TESSELATION_FACTOR; j++)
		output.InsideTesselationFactor[j] = output.InsideTesselationFactor[0];

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
