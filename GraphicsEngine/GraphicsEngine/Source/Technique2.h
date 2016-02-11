#pragma once

#include "VertexShader.h"
#include "PixelShader.h"
#include "HullShader.h"
#include "DomainShader.h"
#include "RasterizerState.h"

namespace GraphicsEngine
{
	template<typename TupleType, typename FunctionType>
	void for_each(TupleType&&, FunctionType, std::integral_constant<size_t, std::tuple_size<typename std::remove_reference<TupleType>::type >::value>)
	{
	}

	template<std::size_t I, typename TupleType, typename FunctionType, typename = typename std::enable_if<I != std::tuple_size<typename std::remove_reference<TupleType>::type>::value>::type>
	void for_each(TupleType&& t, FunctionType f, std::integral_constant<size_t, I>)
	{
		f(std::get<I>(t));
		for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, I + 1>());
	}

	template<typename TupleType, typename FunctionType>
	void for_each(TupleType&& t, FunctionType f)
	{
		for_each(std::forward<TupleType>(t), f, std::integral_constant<size_t, 0>());
	}

	template<size_t ConstantBufferCount, size_t SamplerStateCount>
	class TechniqueArrays2
	{
	public:
		explicit TechniqueArrays2(
			void(__stdcall ID3D11DeviceContext1::* const& setConstantBuffers)(UINT, UINT, ID3D11Buffer* const*),
			void(__stdcall ID3D11DeviceContext1::* const& setSamplers)(UINT, UINT, ID3D11SamplerState* const*)
			) :
			m_setConstantBuffers(setConstantBuffers),
			m_setSamplers(setSamplers)
		{
			for (size_t i = 0; i < m_constantBuffers.size(); i++)
				m_constantBuffers[i] = nullptr;

			for (size_t i = 0; i < m_samplerStates.size(); i++)
				m_samplerStates[i] = nullptr;
		}

		void SetConstantBuffers(ID3D11DeviceContext1* d3dDeviceContext) const
		{
			(d3dDeviceContext->*m_setConstantBuffers)(0, m_constantBuffers.size(), m_constantBuffers.data());
		}
		void SetSamplerStates(ID3D11DeviceContext1* d3dDeviceContext) const
		{
			(d3dDeviceContext->*m_setSamplers)(0, m_samplerStates.size(), m_samplerStates.data());
		}

	private:
		std::array<ID3D11Buffer*, ConstantBufferCount> m_constantBuffers;
		void(__stdcall ID3D11DeviceContext1::* const& m_setConstantBuffers)(UINT, UINT, ID3D11Buffer* const*);

		std::array<ID3D11SamplerState*, SamplerStateCount> m_samplerStates;
		void(__stdcall ID3D11DeviceContext1::* const& m_setSamplers)(UINT, UINT, ID3D11SamplerState* const*);
	};

	template<size_t ConstantBufferCount, size_t SamplerStateCount>
	class VSTechniqueArray : public TechniqueArrays2<ConstantBufferCount, SamplerStateCount>
	{
	public:
		VSTechniqueArray() :
			TechniqueArrays2(
				&ID3D11DeviceContext1::VSSetConstantBuffers,
				&ID3D11DeviceContext1::VSSetSamplers
				)
		{
		}
	};
	template<size_t ConstantBufferCount, size_t SamplerStateCount>
	class PSTechniqueArray : public TechniqueArrays2<ConstantBufferCount, SamplerStateCount>
	{
	public:
		PSTechniqueArray() :
			TechniqueArrays2(
				&ID3D11DeviceContext1::PSSetConstantBuffers,
				&ID3D11DeviceContext1::PSSetSamplers
				)
		{
		}
	};

	template<typename... Arrays>
	class Technique2
	{
	public:
		Technique2(const VertexShader* m_vertex_shader, const HullShader* m_hull_shader, const DomainShader* m_domain_shader, const PixelShader* m_pixel_shader, const RasterizerState* m_rasterizer_state) :
			m_vertexShader(m_vertex_shader),
			m_hullShader(m_hull_shader),
			m_domainShader(m_domain_shader),
			m_pixelShader(m_pixel_shader),
			m_rasterizerState(m_rasterizer_state)
		{
		}

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const
		{
			// Set vertex shader:
			if (m_vertexShader)
				m_vertexShader->Set(d3dDeviceContext);
			else
				d3dDeviceContext->VSSetShader(nullptr, nullptr, 0);

			// Set hull shader:
			if (m_hullShader)
				m_hullShader->Set(d3dDeviceContext);
			else
				d3dDeviceContext->HSSetShader(nullptr, nullptr, 0);

			// Set domain shader:
			if (m_domainShader)
				m_domainShader->Set(d3dDeviceContext);
			else
				d3dDeviceContext->DSSetShader(nullptr, nullptr, 0);

			// Set pixel shader:
			if (m_pixelShader)
				m_pixelShader->Set(d3dDeviceContext);
			else
				d3dDeviceContext->PSSetShader(nullptr, nullptr, 0);

			// Set rasterizer state:
			if (m_rasterizerState)
				m_rasterizerState->Set(d3dDeviceContext);
			else
				d3dDeviceContext->RSSetState(nullptr);

			for_each(m_arrays, [&](const auto& array) { array.SetConstantBuffers(d3dDeviceContext); });
			for_each(m_arrays, [&](const auto& array) { array.SetSamplerStates(d3dDeviceContext); });
		}

	protected:
		const VertexShader* m_vertexShader;
		const HullShader* m_hullShader;
		const DomainShader* m_domainShader;
		const PixelShader* m_pixelShader;
		const RasterizerState* m_rasterizerState;
		std::tuple<Arrays...> m_arrays;
	};
}
