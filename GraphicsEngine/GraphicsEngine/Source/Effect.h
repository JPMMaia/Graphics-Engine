﻿#pragma once

#include "VertexShader.h"
#include "HullShader.h"
#include "DomainShader.h"
#include "PixelShader.h"
#include "Technique2.h"

#include <array>

namespace GraphicsEngine
{
	namespace EffectTypes
	{
		class VSEffect
		{
		public:
			VSEffect() noexcept;

			template<size_t ArraySize>
			explicit VSEffect(ID3D11Device* d3dDevice, const std::wstring& filename, const std::array<D3D11_INPUT_ELEMENT_DESC, ArraySize>& vertexDesc);

			const VertexShader& GetShader() const noexcept;

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
			HSEffect() noexcept;
			explicit HSEffect(ID3D11Device* d3dDevice, const std::wstring& filename);

			const HullShader& GetShader() const noexcept;

		protected:
			HullShader m_hullShader;
		};
		class DSEffect
		{
		public:
			DSEffect() noexcept;
			DSEffect(ID3D11Device* d3dDevice, const std::wstring& filename);

			const DomainShader& GetShader() const noexcept;

		protected:
			DomainShader m_domainShader;
		};
		class PSEffect
		{
		public:
			PSEffect() noexcept;
			PSEffect(ID3D11Device* d3dDevice, const std::wstring& filename);

			const PixelShader& GetShader() const noexcept;

		protected:
			PixelShader m_pixelShader;
		};
	}

	template<typename... Shaders>
	class Effect : public Shaders...
	{
	public:
		Effect();
		explicit Effect(Shaders&&... shaders, Technique2&& technique);

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const;

	protected:
		Technique2 m_technique;
	};

	template <typename ... Shaders>
	Effect<Shaders...>::Effect()
	{
	}

	template <typename ... Shaders>
	Effect<Shaders...>::Effect(Shaders&&... shaders, Technique2&& technique) :
		Shaders(std::forward<Shaders>(shaders))...,
		m_technique(std::forward<Technique2>(technique))
	{
		std::array<int, sizeof...(Shaders)>(
		{ (m_technique.SetShader(&static_cast<Shaders*>(this)->GetShader()), 0)... }
		);
	}

	template <typename ... Shaders>
	void Effect<Shaders...>::Set(ID3D11DeviceContext1* d3dDeviceContext) const
	{
		m_technique.Set(d3dDeviceContext);
	}
}