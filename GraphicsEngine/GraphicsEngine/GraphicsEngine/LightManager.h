#pragma once

#include "Light.h"

#include <vector>

namespace GraphicsEngine
{
	class LightManager
	{
	public:
		void AddLight(std::unique_ptr<Light>&& light);

		template<typename IteratorType>
		void Fill(IteratorType begin, IteratorType end) const
		{
			auto lightCount = std::distance(begin, end);

			for (SIZE_T i = 0; begin != end && i < m_directionalLights.size(); ++i)
				*begin++ = m_directionalLights[i]->GetLightData();

			for (SIZE_T i = 0; begin != end && i < m_pointLights.size(); ++i)
				*begin++ = m_pointLights[i]->GetLightData();

			for (SIZE_T i = 0; begin != end && i < m_spotLights.size(); ++i)
				*begin++ = m_spotLights[i]->GetLightData();
		}

		const DirectX::XMFLOAT4& GetAmbientLight() const;
		void SetAmbientLight(const DirectX::XMFLOAT4& ambientLight);

		const std::vector<Light*>& GetCastShadowsLights() const;

	private:
		std::vector<std::unique_ptr<Light>> m_lights;
		std::vector<Light*> m_directionalLights;
		std::vector<Light*> m_pointLights;
		std::vector<Light*> m_spotLights;
		std::vector<Light*> m_castShadowsLights;
		DirectX::XMFLOAT4 m_ambientLight;
	};
}
