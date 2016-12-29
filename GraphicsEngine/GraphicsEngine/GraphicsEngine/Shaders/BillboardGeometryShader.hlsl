#include "LightingUtils.hlsli"
#include "PassData.hlsli"

struct VertexOutput
{
    float3 CenterW : POSITION;
    float2 ExtentsW : TEXCOORD;
};
struct GeometryOutput
{
    float3 PositionW : POSITION0;
    float4 PositionH : SV_POSITION;
    float4 ShadowPositionH : POSITION1;
    float3 NormalW : NORMAL;
    float3 TangentW : TANGENT;
    float2 TextureCoordinates : TEXCOORD;
};

[maxvertexcount(4)]
void main(point VertexOutput input[1], inout TriangleStream<GeometryOutput> outputStream)
{
    float3 centerW = input[0].CenterW;
    float2 extentsW = input[0].ExtentsW;

    // Create an orthonormal basis with the Z vector pointing to the camera:
    float3 normalW = normalize(EyePositionW - centerW);
    float3 bitangentW = float3(0.0f, 1.0f, 0.0f);
    float3 tangentW = cross(bitangentW, normalW);

    // Create a rectangle facing the camera:
    {
        GeometryOutput vertex;

        // TODO shadow 
        vertex.ShadowPositionH = float4(0.0f, 0.0f, 0.0f, 1.0f);
        vertex.NormalW = normalW;
        vertex.TangentW = tangentW;

        // Left Up:
        vertex.PositionW = centerW + tangentW * extentsW.x + bitangentW * extentsW.y;
        vertex.PositionH = mul(float4(vertex.PositionW, 1.0f), ViewProjectionMatrix);
        vertex.TextureCoordinates = float2(0.0f, 0.0f);
        outputStream.Append(vertex);

        // Right Up:
        vertex.PositionW = centerW - tangentW * extentsW.x + bitangentW * extentsW.y;
        vertex.PositionH = mul(float4(vertex.PositionW, 1.0f), ViewProjectionMatrix);
        vertex.TextureCoordinates = float2(1.0f, 0.0f);
        outputStream.Append(vertex);

        // Left Down:
        vertex.PositionW = centerW + tangentW * extentsW.x - bitangentW * extentsW.y;
        vertex.PositionH = mul(float4(vertex.PositionW, 1.0f), ViewProjectionMatrix);
        vertex.TextureCoordinates = float2(0.0f, 1.0f);
        outputStream.Append(vertex);

        // Right Down:
        vertex.PositionW = centerW - tangentW * extentsW.x - bitangentW * extentsW.y;
        vertex.PositionH = mul(float4(vertex.PositionW, 1.0f), ViewProjectionMatrix);
        vertex.TextureCoordinates = float2(1.0f, 1.0f);
        outputStream.Append(vertex);
    }
};