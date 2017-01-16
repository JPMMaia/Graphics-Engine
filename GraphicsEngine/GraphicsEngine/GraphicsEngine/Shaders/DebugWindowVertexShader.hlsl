struct VertexInput
{
    float3 PositionL : POSITION;
    float2 TextureCoordinates : TEXCOORD;
};
struct VertexOutput
{
    float4 PositionH : SV_POSITION;
    float2 TextureCoordinates : TEXCOORD;
};

VertexOutput main(VertexInput input)
{
    VertexOutput output;

    // Input position already specified in NDC coordinates:
    output.PositionH = float4(input.PositionL, 1.0f);

    // Set texture coordinates:
    output.TextureCoordinates = input.TextureCoordinates;

    return output;
}