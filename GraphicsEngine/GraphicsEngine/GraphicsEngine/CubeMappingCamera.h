#pragma once

namespace GraphicsEngine
{
	class CubeMappingCamera
	{
	public:
		CubeMappingCamera() = default;
		CubeMappingCamera(const DirectX::XMFLOAT3& position);

		const DirectX::XMFLOAT4X4& GetViewMatrix(size_t index) const;
		const DirectX::XMFLOAT4X4& GetProjectionMatrix() const;

	private:
		void BuildMatrices(const DirectX::XMFLOAT3& position);

	private:
		std::array<DirectX::XMFLOAT4X4, 6> m_viewMatrices;
		DirectX::XMFLOAT4X4 m_projectionMatrix;
	};
}
