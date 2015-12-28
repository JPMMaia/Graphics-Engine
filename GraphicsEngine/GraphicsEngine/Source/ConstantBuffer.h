#pragma once

#include "Buffer.h"

namespace DX
{
	void ThrowIfFailed(HRESULT hr);
}

namespace GraphicsEngine
{
	template<class BufferType>
	class ConstantBuffer
	{
	public:
		ConstantBuffer();
		ConstantBuffer(ID3D11Device* d3dDevice, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);

		void Initialize(ID3D11Device* d3dDevice, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);
		void Shutdown();

		void Update(ID3D11DeviceContext1* d3dDeviceContext, const BufferType& constantBufferData);

		void VSSet(ID3D11DeviceContext1* d3dDeviceContext, uint32_t slot);
		void PSSet(ID3D11DeviceContext1* d3dDeviceContext, uint32_t slot);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> m_constantBuffer;
	};

	template <class BufferType>
	ConstantBuffer<BufferType>::ConstantBuffer()
	{
	}

	template <class BufferType>
	ConstantBuffer<BufferType>::ConstantBuffer(ID3D11Device* d3dDevice, D3D11_USAGE usage)
	{
		Initialize(d3dDevice, usage);
	}

	template <class BufferType>
	void ConstantBuffer<BufferType>::Initialize(ID3D11Device* d3dDevice, D3D11_USAGE usage)
	{
		// Create constant buffer description:
		CD3D11_BUFFER_DESC constantBufferDesc(sizeof(BufferType), D3D11_BIND_CONSTANT_BUFFER, usage);

		// Create constant buffer:
		DX::ThrowIfFailed(
			d3dDevice->CreateBuffer(
				&constantBufferDesc,
				nullptr,
				m_constantBuffer.GetAddressOf()
				)
			);
	}

	template <class BufferType>
	void ConstantBuffer<BufferType>::Shutdown()
	{
		m_constantBuffer.Reset();
	}

	template <class BufferType>
	void ConstantBuffer<BufferType>::Update(ID3D11DeviceContext1* d3dDeviceContext, const BufferType& constantBufferData)
	{
		// Prepare the constant buffer to send it to the graphics device:
		d3dDeviceContext->UpdateSubresource1(
			m_constantBuffer.Get(),
			0,
			nullptr,
			&constantBufferData,
			0,
			0,
			0
			);
	}

	template <class BufferType>
	void ConstantBuffer<BufferType>::VSSet(ID3D11DeviceContext1* d3dDeviceContext, uint32_t slot)
	{
		// Send the constant buffer to the graphics device:
		d3dDeviceContext->VSSetConstantBuffers1(
			slot,
			1,
			m_constantBuffer.GetAddressOf(),
			nullptr,
			nullptr
			);
	}

	template <class BufferType>
	void ConstantBuffer<BufferType>::PSSet(ID3D11DeviceContext1* d3dDeviceContext, uint32_t slot)
	{
		// Send the constant buffer to the graphics device:
		d3dDeviceContext->PSSetConstantBuffers1(
			slot,
			1,
			m_constantBuffer.GetAddressOf(),
			nullptr,
			nullptr
			);
	}
}
