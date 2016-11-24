float2 BilinearInterpolate(float2 v0, float2 v1, float2 v2, float2 v3, float2 domain)
{
    float2 horizontal0 = lerp(v0, v1, domain.x);
    float2 horizontal1 = lerp(v2, v3, domain.x);
    return lerp(horizontal0, horizontal1, 1 - domain.y);
}

float3 BilinearInterpolate(float3 v0, float3 v1, float3 v2, float3 v3, float2 domain)
{
    float3 horizontal0 = lerp(v0, v1, domain.x);
    float3 horizontal1 = lerp(v2, v3, domain.x);
    return lerp(horizontal0, horizontal1, 1 - domain.y);
}
