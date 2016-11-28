cbuffer PassData : register(b2)
{
	float4x4 ViewMatrix;
	float4x4 InverseViewMatrix;
	float4x4 ProjectionMatrix;
	float4x4 InverseProjectionMatrix;
	float4x4 ViewProjectionMatrix;
	float4x4 InverseViewProjectionMatrix;
	float3 EyePositionW;
	float TerrainDisplacementScalarY;
	float2 RenderTargetSize;
	float2 InverseRenderTargetSize;
	float NearZ;
	float FarZ;
	float TotalTime;
	float DeltaTime;
	float4 FogColor;
	float FogStart;
	float FogRange;
    float2 Padding1;
    float MaxTesselationDistance;
    float MaxTesselationFactor;
    float MinTesselationDistance;
    float MinTesselationFactor;
    float2 TexelSize;
    float TiledTexelScale;
    float Padding2;
    float4 SkyDomeColors[2];
	float4 AmbientLight;
	Light Lights[MAX_NUM_LIGHTS];
};
