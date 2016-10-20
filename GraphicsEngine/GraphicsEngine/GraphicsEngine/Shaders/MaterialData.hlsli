cbuffer MaterialData : register(b1)
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Roughness;
    float4x4 MaterialTransform;
};