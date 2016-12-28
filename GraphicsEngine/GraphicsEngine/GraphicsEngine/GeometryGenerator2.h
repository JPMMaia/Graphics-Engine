#pragma once

namespace GraphicsEngine
{
	class GeometryGenerator2
	{
	public:
		
		struct Vertex
		{
			Vertex() = default;
			Vertex(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& normal, const DirectX::XMFLOAT3& tangentU, const DirectX::XMFLOAT2& textureCoordinates);

			DirectX::XMFLOAT3 Position;
			DirectX::XMFLOAT3 Normal;
			DirectX::XMFLOAT2 TextureCoordinates;
			DirectX::XMFLOAT3 TangentU;
		};

		struct MeshData
		{
			std::vector<Vertex> Vertices;
			std::vector<uint32_t> Indices;
		};

	public:
		
		static MeshData CreateRectangle(float originX, float originY, float width, float height, float depth);

	private:

	};
}
