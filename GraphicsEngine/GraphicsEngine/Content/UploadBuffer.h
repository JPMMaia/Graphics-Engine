#pragma once

#include "d3dx12.h"
#include "DirectXHelper.h"

namespace GraphicsEngine
{
	template<typename Type>
	class UploadBuffer
	{
	public:
		UploadBuffer(ID3D12Device* d3dDevice, size_t elementCount, bool isConstantBuffer) :
			m_isConstantBuffer(isConstantBuffer)
		{
			// Constant buffers need to be multiples of 256 bytes:
			m_elementByteSize = isConstantBuffer ? DX::CalculateConstantBufferByteSize(sizeof(Type)) : sizeof(Type);

			auto uploadHeapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE::D3D12_HEAP_TYPE_UPLOAD);
			auto bufferDescription = CD3DX12_RESOURCE_DESC::Buffer(m_elementByteSize * elementCount);
			DX::ThrowIfFailed(
				d3dDevice->CreateCommittedResource(
					&uploadHeapProperties,
					D3D12_HEAP_FLAGS::D3D12_HEAP_FLAG_NONE,
					&bufferDescription,
					D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_GENERIC_READ,
					nullptr,
					IID_PPV_ARGS(m_uploadBuffer.GetAddressOf())
					)
				);

			m_uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedData));

			// We do not need to unmap until we are done with the resource.
			// However, we must not write to the resource while it is in use by the GPU (so we must use synchronization techniques).
		}

		UploadBuffer(const UploadBuffer& rhs) = delete;
		UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

		~UploadBuffer()
		{
			if (m_uploadBuffer != nullptr)
			{
				m_uploadBuffer->Unmap(0, nullptr);
				m_uploadBuffer = nullptr;
			}
				
			m_mappedData = nullptr;
		}

		ID3D12Resource* GetResource() const
		{
			return m_uploadBuffer.Get();
		}

		void CopyData(int elementIndex, const Type& data)
		{
			memcpy_s(&m_mappedData[elementIndex * m_elementByteSize], sizeof(Type), &data, sizeof(Type));
		}

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadBuffer;
		BYTE* m_mappedData = nullptr;

		size_t m_elementByteSize = 0;
		bool m_isConstantBuffer = false;
	};
}
