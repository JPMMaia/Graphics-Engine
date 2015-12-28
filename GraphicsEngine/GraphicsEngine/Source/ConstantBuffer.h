#pragma once

#include "Buffer.h"

namespace GraphicsEngine
{
	class ConstantBuffer
	{
	public:
		ConstantBuffer();
		ConstantBuffer(ID3D11Device* d3dDevice, uint32_t bufferSize, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);

		void Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize, D3D11_USAGE usage = D3D11_USAGE_DEFAULT);
		void Reset();

		template<class BufferType>
		void Update(ID3D11DeviceContext1* d3dDeviceContext, const BufferType& constantBufferData);

		void VSSet(ID3D11DeviceContext1* d3dDeviceContext, uint32_t slot);
		void PSSet(ID3D11DeviceContext1* d3dDeviceContext, uint32_t slot);

	private:
		Buffer m_constantBuffer;
	};

	template <class BufferType>
	void ConstantBuffer::Update(ID3D11DeviceContext1* d3dDeviceContext, const BufferType& constantBufferData)
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
}