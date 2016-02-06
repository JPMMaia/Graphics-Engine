#pragma once

#include <d3d11_4.h>
#include <vector>

namespace GraphicsEngine
{
	template<typename ElementType>
	class ResourceArrayBase
	{
	public:
		ResourceArrayBase();
		ResourceArrayBase(uint32_t size);
		virtual ~ResourceArrayBase();

		void Initialize(uint32_t size);
		void Reset();

		void SetSize(uint32_t size);
		void SetElement(ElementType&& element, uint32_t slot);

		virtual void VSSet(ID3D11DeviceContext1* d3dDeviceContext) const = 0;
		virtual void HSSet(ID3D11DeviceContext1* d3dDeviceContext) const = 0;
		virtual void DSSet(ID3D11DeviceContext1* d3dDeviceContext) const = 0;
		virtual void PSSet(ID3D11DeviceContext1* d3dDeviceContext) const = 0;

	protected:
		std::vector<ElementType> m_array;
	};

	template <class ElementType>
	ResourceArrayBase<ElementType>::ResourceArrayBase()
	{
	}

	template <class ElementType>
	ResourceArrayBase<ElementType>::ResourceArrayBase(uint32_t size) :
		m_array(size)
	{
	}

	template <class ElementType>
	ResourceArrayBase<ElementType>::~ResourceArrayBase()
	{
	}

	template <class ElementType>
	void ResourceArrayBase<ElementType>::Initialize(uint32_t size)
	{
		SetSize(size);
	}

	template <class ElementType>
	void ResourceArrayBase<ElementType>::Reset()
	{
		SetSize(0);
	}

	template <class ElementType>
	void ResourceArrayBase<ElementType>::SetSize(uint32_t size)
	{
		m_array.Resize(size);
	}

	template <class ElementType>
	void ResourceArrayBase<ElementType>::SetElement(ElementType&& element, uint32_t slot)
	{
		// If slot is out of bounds, then resize vector:
		if (slot + 1 > m_array.size())
			m_array.resize(slot + 1);

		m_array[slot] = element;
	}

	template<typename ElementType>
	class ResourceArray : public ResourceArrayBase<ElementType>
	{
	};

	template<>
	class ResourceArray<ID3D11Buffer*> : public ResourceArrayBase<ID3D11Buffer*>
	{
	public:
		void VSSet(ID3D11DeviceContext1* d3dDeviceContext) const override;
		void HSSet(ID3D11DeviceContext1* d3dDeviceContext) const override;
		void DSSet(ID3D11DeviceContext1* d3dDeviceContext) const override;
		void PSSet(ID3D11DeviceContext1* d3dDeviceContext) const override;
	};
	inline void ResourceArray<struct ID3D11Buffer*>::VSSet(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		if (!m_array.empty())
			d3dDeviceContext->VSSetConstantBuffers(0, m_array.size(), &m_array[0]);
	}
	inline void ResourceArray<struct ID3D11Buffer*>::HSSet(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		if (!m_array.empty())
			d3dDeviceContext->HSSetConstantBuffers(0, m_array.size(), &m_array[0]);
	}
	inline void ResourceArray<struct ID3D11Buffer*>::DSSet(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		if(!m_array.empty())
			d3dDeviceContext->DSSetConstantBuffers(0, m_array.size(), &m_array[0]);
	}
	inline void ResourceArray<struct ID3D11Buffer*>::PSSet(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		if (!m_array.empty())
			d3dDeviceContext->PSSetConstantBuffers(0, m_array.size(), &m_array[0]);
	}

	template<>
	class ResourceArray<ID3D11SamplerState*> : public ResourceArrayBase<ID3D11SamplerState*>
	{
	public:
		void VSSet(ID3D11DeviceContext1* d3dDeviceContext) const override;
		void HSSet(ID3D11DeviceContext1* d3dDeviceContext) const override;
		void DSSet(ID3D11DeviceContext1* d3dDeviceContext) const override;
		void PSSet(ID3D11DeviceContext1* d3dDeviceContext) const override;
	};
	inline void ResourceArray<struct ID3D11SamplerState*>::VSSet(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		if (!m_array.empty())
			d3dDeviceContext->VSSetSamplers(0, m_array.size(), &m_array[0]);
	}
	inline void ResourceArray<struct ID3D11SamplerState*>::HSSet(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		if (!m_array.empty())
			d3dDeviceContext->HSSetSamplers(0, m_array.size(), &m_array[0]);
	}
	inline void ResourceArray<struct ID3D11SamplerState*>::DSSet(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		if (!m_array.empty())
			d3dDeviceContext->DSSetSamplers(0, m_array.size(), &m_array[0]);
	}
	inline void ResourceArray<struct ID3D11SamplerState*>::PSSet(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		if (!m_array.empty())
			d3dDeviceContext->PSSetSamplers(0, m_array.size(), &m_array[0]);
	}

	template<>
	class ResourceArray<ID3D11ShaderResourceView*> : public ResourceArrayBase<ID3D11ShaderResourceView*>
	{
	public:
		void VSSet(ID3D11DeviceContext1* d3dDeviceContext) const override;
		void HSSet(ID3D11DeviceContext1* d3dDeviceContext) const override;
		void DSSet(ID3D11DeviceContext1* d3dDeviceContext) const override;
		void PSSet(ID3D11DeviceContext1* d3dDeviceContext) const override;
	};
	inline void ResourceArray<struct ID3D11ShaderResourceView*>::VSSet(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		if (!m_array.empty())
			d3dDeviceContext->VSSetShaderResources(0, m_array.size(), &m_array[0]);
	}
	inline void ResourceArray<struct ID3D11ShaderResourceView*>::HSSet(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		if (!m_array.empty())
			d3dDeviceContext->HSSetShaderResources(0, m_array.size(), &m_array[0]);
	}
	inline void ResourceArray<struct ID3D11ShaderResourceView*>::DSSet(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		if (!m_array.empty())
			d3dDeviceContext->DSSetShaderResources(0, m_array.size(), &m_array[0]);
	}
	inline void ResourceArray<struct ID3D11ShaderResourceView*>::PSSet(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		if (!m_array.empty())
			d3dDeviceContext->PSSetShaderResources(0, m_array.size(), &m_array[0]);
	}
}