#include "LightingUtils.hlsli"
#include "PassData.hlsli"

struct VertexOutput
{
    float3 CenterW : POSITION;
    float2 ExtentsW : TEXCOORD0;
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

        // Shadows not used:
        vertex.ShadowPositionH = float4(0.0f, 0.0f, 0.0f, 1.0f);

        // Setting normal and tangent:
        vertex.NormalW = normalW;
        vertex.TangentW = tangentW;

        const uint offsetsSize = 4;
        float2 offsets[offsetsSize] =
        {
            float2(+1.0f, +1.0f),
            float2(-1.0f, +1.0f),
            float2(+1.0f, -1.0f),
            float2(-1.0f, -1.0f)
        };

        float2x2 rotationMatrix = float2x2(
                GrassTransformMatrix._11, GrassTransformMatrix._12,
                GrassTransformMatrix._21, GrassTransformMatrix._22
            );

        for (uint i = 0; i < offsetsSize; ++i)
        {
            float2 offset = offsets[i];

            float3 positionL = offset.x * tangentW * extentsW.x + offset.y * bitangentW * extentsW.y;
            positionL = float3(mul(positionL.xy, rotationMatrix), positionL.z);
            vertex.PositionW = centerW + positionL;
            vertex.PositionH = mul(float4(vertex.PositionW, 1.0f), ViewProjectionMatrix);
            vertex.TextureCoordinates = float2(0.5f, 0.5f) + 0.5f * float2(-offset.x, -offset.y);
            outputStream.Append(vertex);
        }
    }
};