#include "LightingUtils.hlsli"
#include "MaterialData.hlsli"
#include "PassData.hlsli"

struct VertexOutput
{
    float4 PositionH : SV_POSITION;
    float3 PositionW : POSITION;
    float3 NormalW : NORMAL;
};

float4 main(VertexOutput input) : SV_TARGET
{
    // Interpolating rasterization process can change the magnitude of the normal vector
    input.NormalW = normalize(input.NormalW);

    float3 toEyeDirection = normalize(EyePositionW - input.PositionW);

    float4 ambientStrength = AmbientLight * DiffuseAlbedo;

    float shininess = 1.0f - Roughness;

    Material material =
    {
        DiffuseAlbedo,
        FresnelR0,
        shininess
    };

    float4 directLighting = ComputeLighting(Lights, material, input.PositionW, input.NormalW, toEyeDirection);
    
    float4 color = ambientStrength + directLighting;
    color.a = DiffuseAlbedo.a;

    return color;
}
