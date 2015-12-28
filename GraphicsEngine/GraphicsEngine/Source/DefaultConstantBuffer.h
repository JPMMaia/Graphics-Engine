#pragma once

#include "ConstantBuffer.h"

namespace GraphicsEngine
{
	class DefaultConstantBuffer : public ConstantBuffer
	{
	public:
		DefaultConstantBuffer();
		DefaultConstantBuffer(ID3D11Device* d3dDevice, uint32_t bufferSize);

		void Initialize(ID3D11Device* d3dDevice, uint32_t bufferSize) override;

		void Update(ID3D11DeviceContext1* d3dDeviceContext, const void* constantBufferData) const override;
	};
}
