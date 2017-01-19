#pragma once

namespace GraphicsEngine
{
	class CubeMappingCamera
	{
	public:
		CubeMappingCamera() = default;
		explicit CubeMappingCamera(DirectX::FXMVECTOR position);

		void SetPosition(DirectX::FXMVECTOR position);

		DirectX::XMVECTOR GetPosition() const;
		const DirectX::XMMATRIX& GetViewMatrix(size_t index) const;
		const DirectX::XMMATRIX& GetProjectionMatrix() const;
		float GetNearZ() const;
		float GetFarZ() const;

	private:
		void BuildMatrices();

	private:
		DirectX::XMVECTOR m_position;
		std::array<DirectX::XMMATRIX, 6> m_viewMatrices;
		DirectX::XMMATRIX m_projectionMatrix;
		float m_nearZ = 0.01f;
		float m_farZ = 1024.0f;
	};
}
