#pragma once

#include <DirectXCollision.h>
#include <vector>

namespace GraphicsEngine
{
	class Ray
	{
	public:
		Ray(DirectX::FXMVECTOR origin, DirectX::FXMVECTOR direction);
		Ray(const DirectX::XMFLOAT3& origin, const DirectX::XMFLOAT3& direction);

		template<typename VertexType, typename IndexType>
		bool IntersectsTriangleMesh(const std::vector<VertexType>& vertices, const std::vector<IndexType>& indices, float& distance)
		{
			using namespace DirectX;

			// For each triangle:
			for(size_t i = 0; i < indices.size(); i += 3)
			{
				auto v0 = XMLoadFloat3(&vertices[indices[i + 0]].Position);
				auto v1 = XMLoadFloat3(&vertices[indices[i + 1]].Position);
				auto v2 = XMLoadFloat3(&vertices[indices[i + 2]].Position);

				// Test
				if (TriangleTests::Intersects(m_origin, m_direction, v0, v1, v2, distance))
					return true;
			}

			return false;
		}

		DirectX::XMVECTOR CalculatePoint(float distance) const;

	private:
		DirectX::XMVECTOR m_origin;
		DirectX::XMVECTOR m_direction;
	};
}
