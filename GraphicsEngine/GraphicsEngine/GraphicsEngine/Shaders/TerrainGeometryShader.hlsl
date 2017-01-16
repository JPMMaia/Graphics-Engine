struct DomainOutput
{
    float3 PositionW : POSITION0;
    float4 PositionH : SV_POSITION;
    float4 ShadowPositionH : POSITION1;
    float2 TextureCoordinates : TEXCOORD0;
    float2 TiledTextureCoordinates : TEXCOORD1;
};

struct StreamOutput
{
    float3 PositionW : POSITION0;
};

[maxvertexcount(3)]
void main(triangle DomainOutput input[3], inout TriangleStream<StreamOutput> triangleStream)
{   
    for (uint i = 0; i < 3; ++i)
    {
        StreamOutput output;

        output.PositionW = input[i].PositionW;

        triangleStream.Append(output);
    }
    
    triangleStream.RestartStrip();
}