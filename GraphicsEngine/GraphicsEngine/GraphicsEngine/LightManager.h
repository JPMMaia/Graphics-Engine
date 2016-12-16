#pragma once

#include "Light.h"

#include <vector>
#include "ShaderBufferTypes.h"

namespace GraphicsEngine
{
	class LightManager
	{
	public:
		void AddDirectionalLight(const DirectX::XMFLOAT3& strength, const DirectX::XMFLOAT3& direction);
		void AddPointLight(const DirectX::XMFLOAT3& strength, float falloffStart, float falloffEnd, const DirectX::XMFLOAT3& position);
		void AddSpotLight(const DirectX::XMFLOAT3& strength, float falloffStart, const DirectX::XMFLOAT3& direction, float falloffEnd, const DirectX::XMFLOAT3& position, float spotPower);

		template<typename IteratorType>
		void Fill(IteratorType begin, IteratorType end) const
		{
			auto lightCount = std::distance(begin, end);

			for (SIZE_T i = 0; begin != end && i < m_directionalLights.size(); ++i, ++begin)
				*begin = m_directionalLights[i++];

			for (SIZE_T i = 0; begin != end && i < m_pointLights.size(); ++i, ++begin)
				*begin = m_pointLights[i++];

			for (SIZE_T i = 0; begin != end && i < m_spotLights.size(); ++i, ++begin)
				*begin = m_spotLights[i++];
		}

		const DirectX::XMFLOAT4& GetAmbientLight() const;
		void SetAmbientLight(const DirectX::XMFLOAT4& ambientLight);

	private:
		std::vector<ShaderBufferTypes::LightData> m_directionalLights;
		std::vector<ShaderBufferTypes::LightData> m_pointLights;
		std::vector<ShaderBufferTypes::LightData> m_spotLights;
		DirectX::XMFLOAT4 m_ambientLight;
	};
}
