#pragma once

#include "BaseGeometryBuffer.h"

namespace GraphicsEngine
{
	class D3DBase;

	class IndexBuffer : public BaseGeometryBuffer
	{
	public:
		IndexBuffer() = default;
		explicit IndexBuffer(const D3DBase& d3dBase, const void* indices, size_t indexCount, size_t indexByteSize, DXGI_FORMAT indexFormat);
		IndexBuffer(const IndexBuffer& rhs) = default;
		IndexBuffer& operator=(const IndexBuffer& rhs) = default;

		DXGI_FORMAT GetIndexFormat() const;

	private:
		DXGI_FORMAT m_indexFormat = DXGI_FORMAT::DXGI_FORMAT_R32_UINT;
	};
}
