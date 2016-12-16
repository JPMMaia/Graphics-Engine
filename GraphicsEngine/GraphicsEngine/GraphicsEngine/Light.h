#pragma once

#include "ShaderBufferTypes.h"

namespace GraphicsEngine
{
	class Light
	{
	public:
		DirectX::XMFLOAT4X4 GetViewMatrix(const DirectX::XMFLOAT3& focusPosition) const;
		DirectX::XMFLOAT4X4 GetOrthographicMatrix(float viewWidth, float viewHeight, float nearZ, float farZ) const;

	private:
		ShaderBufferTypes::LightData m_lightData;
	};
}
