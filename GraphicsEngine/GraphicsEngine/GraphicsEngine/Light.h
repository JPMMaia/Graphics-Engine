#pragma once

#include "ShaderBufferTypes.h"

#include <DirectXCollision.h>

namespace GraphicsEngine
{
	__declspec(align(16))
	class Light
	{
	public:
		enum class Type
		{
			Directional,
			Point,
			Spot
		};

	public:
		static Light CreateDirectionalLight(const DirectX::XMFLOAT3& strength, const DirectX::XMFLOAT3& direction, bool castShadows);
		static Light CreatePointLight(const DirectX::XMFLOAT3& strength, float falloffStart, float falloffEnd, const DirectX::XMFLOAT3& position);
		static Light CreateSpotLight(const DirectX::XMFLOAT3& strength, float falloffStart, const DirectX::XMFLOAT3& direction, float falloffEnd, const DirectX::XMFLOAT3& position, float spotPower);

	public:
		void UpdateMatrices(const DirectX::BoundingSphere& sceneBounds, DirectX::FXMVECTOR cameraPosition);
		
		void RotateRollPitchYaw(float pitchRadians, float yawRadians, float rollRadians);

		Type GetType() const;
		const ShaderBufferTypes::LightData& GetLightData() const;
		bool CastShadows() const;
		const DirectX::XMMATRIX& GetViewMatrix() const;
		const DirectX::XMMATRIX& GetProjectionMatrix() const;
		const DirectX::XMMATRIX& GetShadowMatrix() const;
		
		void SetStrength(const DirectX::XMFLOAT3& strength);
		void SetDirection(const DirectX::XMFLOAT3& direction);
		void SetPosition(const DirectX::XMFLOAT3& position);

	private:
		Light() = default;

	private:
		Type m_type;
		ShaderBufferTypes::LightData m_lightData;
		bool m_castShadows = false;
		DirectX::XMMATRIX m_viewMatrix;
		DirectX::XMMATRIX m_projectionMatrix;
		DirectX::XMMATRIX m_shadowMatrix;
	};
}
