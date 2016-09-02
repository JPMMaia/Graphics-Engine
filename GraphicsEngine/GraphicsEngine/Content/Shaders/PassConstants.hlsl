#ifndef __PASS_CONSTANTS__
#define __PASS_CONSTANTS__

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
	float4 FogColor;
	float FogStart;
	float FogRange;
	float2 Padding2;
	float4 AmbientLight;
	Light Lights[MaxLights];
};

#endif