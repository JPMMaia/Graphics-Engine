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

/// <Summary>
/// Use the method of the Central Differences to compute an estimate for the derivatives.
/// h(x + h) - h(x - h)
/// -------------------
///        2h
/// </Summary>
void CalculateNormalTangentBitangentFromHeightMap(float2 textureCoordinates, float2 texelSize, Texture2D heightMap, SamplerState linearSampler, out float3 normalW, out float3 tangentW, out float3 bitangentW)
{
    // Calculate the texture coordinates of the neighbor texels:
    float2 leftTextureCoordinates = textureCoordinates + texelSize.x * float2(-1.0f, 0.0f);
    float2 rightTextureCoordinates = textureCoordinates + texelSize.x * float2(1.0f, 0.0f);
    float2 bottomTextureCoordinates = textureCoordinates + texelSize.y * float2(0.0f, -1.0f);
    float2 topTextureCoordinates = textureCoordinates + texelSize.y * float2(0.0f, 1.0f);

    // Sample the height values of the neighbor texels:
    float leftY = heightMap.SampleLevel(linearSampler, leftTextureCoordinates, 0).r;
    float rightY = heightMap.SampleLevel(linearSampler, rightTextureCoordinates, 0).r;
    float bottomY = heightMap.SampleLevel(linearSampler, bottomTextureCoordinates, 0).r;
    float topY = heightMap.SampleLevel(linearSampler, topTextureCoordinates, 0).r;

    // Compute the tangent and bitangent.
    // position = (x, h(x, z), z)
    // tangent = d(position) / dx = (1.0f, d(h(x, z)) / dx, 0.0f)
    // bitangent = d(position) / dz = (0.0f, d(h(x, z)) / dz, 1.0f)
    tangentW = normalize(float3(2.0f * texelSize.x, rightY - leftY, 0.0f));
    bitangentW = normalize(float3(0.0f, topY - bottomY, -2.0f * texelSize.y));

    // The normal can be obtained by the cross product of the tangent and bitangent vectors:
    normalW = normalize(cross(tangentW, bitangentW));
}

float3 NormalSampleToBumpedNormalW(float3 normalSample, float3 normalW, float3 tangentW)
{
    // Map from range [0, 1] to [-1, 1]:
    float3 normalT = (normalSample * 2.0f) - 1.0f;

    // Build orthonormal basis:
    tangentW = normalize(tangentW - dot(tangentW, normalW) * normalW);
    float3 bitangentW = cross(normalW, tangentW);
    float3x3 tangentToWorldMatrix = float3x3(tangentW, bitangentW, normalW);

    // Transform from tangent space to world space:
    float3 bumpedNormalW = mul(normalT, tangentToWorldMatrix);

    return bumpedNormalW;
}

float4 AddFog(float4 color, float distanceToEye, float fogStart, float fogRange, float4 fogColor)
{
    float fogIntensity = saturate((distanceToEye - fogStart) / fogRange);
    return lerp(color, fogColor, fogIntensity);
}