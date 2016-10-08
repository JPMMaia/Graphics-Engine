#pragma once

#include <d3d12.h>

namespace GraphicsEngine
{
	class D3DBase;

	class BaseGeometryBuffer
	{
	protected:
		BaseGeometryBuffer() = default;
		BaseGeometryBuffer(const D3DBase& d3dBase, const void* data, size_t count, size_t elementByteSize);
		BaseGeometryBuffer(const BaseGeometryBuffer& rhs) = default;
		BaseGeometryBuffer& operator=(const BaseGeometryBuffer& rhs) = default;

	public:
		void DisposeUploadBuffer();

		ID3DBlob* GetBufferCPU() const;
		ID3D12Resource* GetBufferGPU() const;
		uint32_t GetBufferByteSize() const;

	protected:
		Microsoft::WRL::ComPtr<ID3DBlob> m_bufferCPU;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_bufferGPU;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadBuffer;
		uint32_t m_bufferByteSize = 0;
	};
}
