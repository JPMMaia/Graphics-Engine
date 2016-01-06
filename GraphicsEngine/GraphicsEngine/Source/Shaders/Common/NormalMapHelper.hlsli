#ifndef _NORMAL_MAP_HELPER_
#define _NORMAL_MAP_HELPER_

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	// Orthonormalize tangent:
	float3 unitTangentW = normalize(tangentW - dot(unitNormalW, tangentW)*unitNormalW);
	
	// Calculate binormal:
	float3 unitBinormalW = cross(unitNormalW, unitTangentW);

	// Build the tangent-binormal-normal orthonormal basis:
	float3x3 tangentBinormalNormalMatrix = float3x3(unitTangentW, unitBinormalW, unitNormalW);

	// Scale and shift normal value from [0, 1] to [-1, 1]:
	float3 bumpedNormalT = 2.0f * normalMapSample - 1.0f;

	// Transform bumped normal from tangent space to world space:
	return mul(bumpedNormalT, tangentBinormalNormalMatrix);
}

#endif