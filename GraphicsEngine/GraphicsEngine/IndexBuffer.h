#pragma once

#include "Buffer.h"

namespace GraphicsEngine
{
	class IndexBuffer
	{
	public:
		IndexBuffer(ID3D11Device* d3dDevice, const std::vector<uint32_t>& indices);

		void Set(ID3D11DeviceContext* d3dDeviceContext) const;

	private:
		Buffer<uint32_t> m_indexBuffer;
	};
}
