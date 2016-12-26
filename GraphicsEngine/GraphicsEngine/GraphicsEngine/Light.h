#pragma once

#include "ShaderBufferTypes.h"

namespace GraphicsEngine
{
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
		static Light CreateDirectionalLight(const DirectX::XMFLOAT3& strength, const DirectX::XMFLOAT3& direction);
		static Light CreateDirectionalCastShadowsLight(const DirectX::XMFLOAT3& strength, const DirectX::XMFLOAT3& direction, const DirectX::XMFLOAT3& position);
		static Light CreatePointLight(const DirectX::XMFLOAT3& strength, float falloffStart, float falloffEnd, const DirectX::XMFLOAT3& position);
		static Light CreateSpotLight(const DirectX::XMFLOAT3& strength, float falloffStart, const DirectX::XMFLOAT3& direction, float falloffEnd, const DirectX::XMFLOAT3& position, float spotPower);

	public:
		DirectX::XMFLOAT4X4 GetViewMatrix() const;
		static DirectX::XMFLOAT4X4 GetOrthographicMatrix(float viewWidth, float viewHeight, float nearZ, float farZ);

		Type GetType() const;
		const ShaderBufferTypes::LightData& GetLightData() const;
		bool CastShadows() const;

	private:
		Light() = default;

	private:
		Type m_type;
		ShaderBufferTypes::LightData m_lightData;
		bool m_castShadows = false;
	};
}
