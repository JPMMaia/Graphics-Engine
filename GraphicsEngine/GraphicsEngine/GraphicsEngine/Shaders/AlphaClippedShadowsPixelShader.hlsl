#include "MaterialData.hlsli"
#include "Samplers.hlsli"

struct VertexOutput
{
    float3 PositionW : POSITION0;
    float4 PositionH : SV_POSITION;
    float4 ShadowPositionH : POSITION1;
    float3 NormalW : NORMAL;
    float2 TextureCoordinates : TEXCOORD;
};

Texture2D DiffuseMap : register(t0);

void main(VertexOutput input)
{
    // Compute diffuse albedo by multiplying the sample from the texture and the diffuse albedo of the material:
    float4 diffuseAlbedo = DiffuseMap.Sample(SamplerAnisotropicClamp, input.TextureCoordinates) * DiffuseAlbedo;
	
	// Clip if alpha is near 0:
	clip(diffuseAlbedo.a - 0.1f);
}
