#pragma once

#include "ResourceArray.h"

namespace GraphicsEngine
{
	template<class ResourceType>
	class TechniqueArrays
	{
	public:
		void Set(ID3D11DeviceContext1* d3dDeviceContext) const;

		void VSSetElement(ResourceType&& element, uint32_t slot);
		void HSSetElement(ResourceType&& element, uint32_t slot);
		void DSSetElement(ResourceType&& element, uint32_t slot);
		void PSSetElement(ResourceType&& element, uint32_t slot);

	private:
		ResourceArray<ResourceType> m_vertexShaderArray;
		ResourceArray<ResourceType> m_hullShaderArray;
		ResourceArray<ResourceType> m_domainShaderArray;
		ResourceArray<ResourceType> m_pixelShaderArray;
	};

	template <class ResourceType>
	void TechniqueArrays<ResourceType>::Set(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		m_vertexShaderArray.VSSet(d3dDeviceContext);
		m_hullShaderArray.HSSet(d3dDeviceContext);
		m_domainShaderArray.DSSet(d3dDeviceContext);
		m_pixelShaderArray.PSSet(d3dDeviceContext);
	}

	template <class ResourceType>
	void TechniqueArrays<ResourceType>::VSSetElement(ResourceType&& element, uint32_t slot)
	{
		m_vertexShaderArray.SetElement(std::forward<ResourceType>(element), slot);
	}

	template <class ResourceType>
	void TechniqueArrays<ResourceType>::HSSetElement(ResourceType&& element, uint32_t slot)
	{
		m_hullShaderArray.SetElement(std::forward<ResourceType>(element), slot);
	}

	template <class ResourceType>
	void TechniqueArrays<ResourceType>::DSSetElement(ResourceType&& element, uint32_t slot)
	{
		m_domainShaderArray.SetElement(std::forward<ResourceType>(element), slot);
	}
	template <class ResourceType>
	void TechniqueArrays<ResourceType>::PSSetElement(ResourceType&& element, uint32_t slot)
	{
		m_pixelShaderArray.SetElement(std::forward<ResourceType>(element), slot);
	}
}
