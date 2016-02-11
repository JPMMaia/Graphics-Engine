#pragma once

#include "VertexShader.h"
#include "PixelShader.h"

namespace GraphicsEngine
{
	class VSTechnique
	{
	public:
		explicit constexpr VSTechnique(const VertexShader* vertexShader) noexcept :
			m_vertexShader(vertexShader)
		{
		}

		void Set(ID3D11DeviceContext* d3dDeviceContext) const;

	protected:
		const VertexShader* m_vertexShader;
	};

	class PSTechnique
	{
	public:
		explicit constexpr PSTechnique(const PixelShader* pixelShader) noexcept :
			m_pixelShader(pixelShader)
		{
		}

		void Set(ID3D11DeviceContext* d3dDeviceContext) const;

	protected:
		const PixelShader* m_pixelShader;
	};

	template<typename... Techniques>
	class Technique2 : public Techniques...
	{
	public:
		explicit Technique2(Techniques&&... techniques);

		void Set(ID3D11DeviceContext* d3dDeviceContext) const
		{
			constexpr auto arraySize = sizeof...(Techniques);
			std::array<int, arraySize>(
				{ 
					(static_cast<const Techniques*>(this)->Set(d3dDeviceContext), 0)... 
				} 
			);
		}
	};

	template <typename ... Techniques>
	Technique2<Techniques...>::Technique2(Techniques&&... techniques):
		Techniques(std::move(techniques))...
	{
	}
}
