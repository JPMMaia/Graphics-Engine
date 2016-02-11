#pragma once

#include "VertexShader.h"
#include "HullShader.h"
#include "DomainShader.h"
#include "PixelShader.h"
#include "Technique.h"

#include <array>

namespace GraphicsEngine
{
	namespace EffectTypes
	{
		class VSEffect
		{
		public:
			template<size_t ArraySize>
			VSEffect(ID3D11Device* d3dDevice, const std::wstring& filename, const std::array<D3D11_INPUT_ELEMENT_DESC, ArraySize>& vertexDesc);

		protected:
			VertexShader m_vertexShader;
		};

		template <size_t ArraySize>
		VSEffect::VSEffect(ID3D11Device* d3dDevice, const std::wstring& filename, const std::array<D3D11_INPUT_ELEMENT_DESC, ArraySize>& vertexDesc) :
			m_vertexShader(d3dDevice, filename, vertexDesc)
		{
		}

		class HSEffect
		{
		public:
			HSEffect(ID3D11Device* d3dDevice, const std::wstring& filename);

		protected:
			HullShader m_hullShader;
		};
		class DSEffect
		{
		public:
			DSEffect(ID3D11Device* d3dDevice, const std::wstring& filename);

		protected:
			DomainShader m_domainShader;
		};
		class PSEffect
		{
		public:
			PSEffect(ID3D11Device* d3dDevice, const std::wstring& filename);

		protected:
			PixelShader m_pixelShader;
		};
	}

	template<typename... Shaders>
	class Effect : public Shaders...
	{
	public:
		explicit Effect(Shaders&&... shaders);

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const;

	protected:
		Technique m_technique;
	};

	template <typename ... Shaders>
	Effect<Shaders...>::Effect(Shaders&&... shaders) : 
		Shaders(std::move(shaders))...
	{
	}

	template <typename ... Shaders>
	void Effect<Shaders...>::Set(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		m_technique.Set(d3dDeviceContext);
	}
}
