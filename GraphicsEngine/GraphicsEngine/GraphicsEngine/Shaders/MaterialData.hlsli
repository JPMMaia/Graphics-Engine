cbuffer MaterialData : register(b1)
{
    float4 DiffuseAlbedo;
    float3 FresnelR0;
    float Roughness;
    float4x4 MaterialTransform;
};
cbuffer MaterialData2 : register(b4)
{
    float4 DiffuseAlbedo2;
    float3 FresnelR02;
    float Roughness2;
    float4x4 MaterialTransform2;
};

