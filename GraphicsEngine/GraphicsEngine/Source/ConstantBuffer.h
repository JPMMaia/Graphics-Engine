#pragma once

#include "Buffer.h"

namespace GraphicsEngine
{
	class ConstantBuffer
	{
	public:
		ConstantBuffer();
		ConstantBuffer(ID3D11Device* d3dDevice, uint32_t bufferSize, D3D11_USAGE usage, uint32_t cpuAccessFlags);
		virtual ~ConstantBuffer();

		virtual void Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize) = 0;
		void Reset();

		virtual void Update(ID3D11DeviceContext1* d3dDeviceContext, const void* constantBufferData) const = 0;

		void VSSet(ID3D11DeviceContext1* d3dDeviceContext, uint32_t slot) const;
		void PSSet(ID3D11DeviceContext1* d3dDeviceContext, uint32_t slot) const;

		ID3D11Buffer* Get() const;
		ID3D11Buffer* const* GetAddressOf() const;
		uint32_t GetBufferSize() const;

	protected:
		void Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize, D3D11_USAGE usage, uint32_t cpuAccessFlags);

	private:
		Buffer m_constantBuffer;
		uint32_t m_bufferSize;
	};
}