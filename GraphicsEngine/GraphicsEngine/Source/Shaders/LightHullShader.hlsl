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
	float EdgeTesselationFactor[3]	: SV_TessFactor;
	float InsideTesselationFactor	: SV_InsideTessFactor;
};

#define NUM_CONTROL_POINTS 3

// Patch Constant Function
TesselationPatch CalcHSPatchConstants(InputPatch<VertexOutput, NUM_CONTROL_POINTS> inputPatch, uint patchID : SV_PrimitiveID)
{
	TesselationPatch output;

	output.EdgeTesselationFactor[0] = 0.5f * (inputPatch[1].TesselationFactor + inputPatch[2].TesselationFactor);
	output.EdgeTesselationFactor[1] = 0.5f * (inputPatch[2].TesselationFactor + inputPatch[0].TesselationFactor);
	output.EdgeTesselationFactor[2] = 0.5f * (inputPatch[0].TesselationFactor + inputPatch[1].TesselationFactor);
	output.InsideTesselationFactor = output.EdgeTesselationFactor[0];

	return output;
}

[domain("tri")]
[partitioning("fractional_odd")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
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
