struct VertexOutput
{
	float3 PositionW : POSITION;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 TextureCoordinate : TEXCOORD0;
	float TesselationFactor : TESSFACTOR;
};

struct HullOutput
{
	float3 PositionW : POSITION;
	float3 NormalW : NORMAL;
	float3 TangentW : TANGENT;
	float2 TextureCoordinate : TEXCOORD0;
};

struct TesselationPatch
{
	float EdgeTesselationFactor[4]		: SV_TessFactor;
	float InsideTesselationFactor[2]	: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 4

// Patch Constant Function
TesselationPatch CalcHSPatchConstants(InputPatch<VertexOutput, NUM_CONTROL_POINTS> inputPatch, uint patchID : SV_PrimitiveID)
{
	TesselationPatch output;

	float tesselationFactor = 1.0f;
	output.EdgeTesselationFactor[0] = tesselationFactor;
	output.EdgeTesselationFactor[1] = tesselationFactor;
	output.EdgeTesselationFactor[2] = tesselationFactor;
	output.EdgeTesselationFactor[3] = tesselationFactor;
	output.InsideTesselationFactor[0] = tesselationFactor;
	output.InsideTesselationFactor[1] = tesselationFactor;

	return output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("CalcHSPatchConstants")]
HullOutput main(
	InputPatch<VertexOutput, NUM_CONTROL_POINTS> inputPatch,
	uint i : SV_OutputControlPointID,
	uint patchID : SV_PrimitiveID )
{
	HullOutput output;

	output.PositionW = inputPatch[i].PositionW;
	output.NormalW = inputPatch[i].NormalW;
	output.TangentW = inputPatch[i].TangentW;
	output.TextureCoordinate = inputPatch[i].TextureCoordinate;

	return output;
}