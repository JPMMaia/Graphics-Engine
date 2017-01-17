#define NUM_CONTROL_POINTS 4

#include "LightingUtils.hlsli"
#include "PassData.hlsli"

struct VertexOutput
{
    float3 PositionW : POSITION;
    float2 TextureCoordinates : TEXCOORD;
};

struct HullOutput
{
    float3 PositionW : POSITION;
    float2 TextureCoordinates : TEXCOORD;
};

struct TesselationPatch
{
    float EdgeTesselationFactor[4] : SV_TessFactor;
    float InsideTesselationFactor[2] : SV_InsideTessFactor;
};

int PowerOfTwo(int x)
{
    int result = 1;
    for (int i = 0; i < x; ++i)
        result *= 2;

    return result;
}

float CalculateTesselationFactor(float3 positionW)
{
	// Calculate the distance from the vertex to the camera:
    float distanceToEye = distance(EyePositionW, positionW);

	// Calculate the tesselation factor:
    float blendFactor = saturate((MinTesselationDistance - distanceToEye) / (MinTesselationDistance - MaxTesselationDistance));   
    return (float) PowerOfTwo((int) lerp(MinTesselationFactor, MaxTesselationFactor, blendFactor));
}

TesselationPatch CalculatePatchConstants(InputPatch<VertexOutput, NUM_CONTROL_POINTS> inputPatch, uint patchID : SV_PrimitiveID)
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
[outputcontrolpoints(NUM_CONTROL_POINTS)]
[patchconstantfunc("CalculatePatchConstants")]
HullOutput main(InputPatch<VertexOutput, NUM_CONTROL_POINTS> inputPatch, uint controlPointID : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HullOutput output;

    output.PositionW = inputPatch[controlPointID].PositionW;
    output.TextureCoordinates = inputPatch[controlPointID].TextureCoordinates;

    return output;
}
