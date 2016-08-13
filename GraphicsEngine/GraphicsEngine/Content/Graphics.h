#pragma once

#include "D3DBase.h"
#include "Technique.h"
#include "Timer.h"
#include "MathHelper.h"
#include "FrameResource.h"
#include "RenderItem.h"

namespace GraphicsEngine
{
	class Graphics
	{
	public:
		explicit Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight);

		void OnResize(uint32_t clientWidth, uint32_t clientHeight);
		void Update(const Timer& timer);
		void Render(const Timer& timer);

		static int GetFrameResourcesCount();
		
	private:
		void InitializeInputLayout();
		void InitializeRootSignature(const D3DBase& d3dBase);
		void InitializeDescriptorHeaps(const D3DBase& d3dBase);
		void InitializeConstantBuffers(const D3DBase& d3dBase);
		void InitializePipelineState(const D3DBase& d3dBase);
		void InitializeGeometry(const D3DBase& d3dBase);

		void UpdateProjectionMatrix();
		void UpdateCamera();
		void UpdateObjectConstantBuffer();
		void UpdateMainPassConstantBuffer(const Timer& timer);

	private:
		D3DBase m_d3d;
		
		std::unique_ptr<MeshGeometry> m_boxGeometry;
		
		DirectX::XMFLOAT4X4 m_modelMatrix = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_viewMatrix = MathHelper::Identity4x4();
		DirectX::XMFLOAT4X4 m_projectionMatrix = MathHelper::Identity4x4();

		static const int s_frameResourcesCount = 3;
		std::vector<std::unique_ptr<FrameResource>> m_frameResources;
		FrameResource* m_currentFrameResource = nullptr;
		int m_currentFrameResourceIndex = 0;

		std::vector<std::unique_ptr<RenderItem>> m_allRenderItems;
		std::vector<RenderItem*> m_opaqueItems;

		ConstantBufferTypes::PassConstants m_passConstants;

		DirectX::XMFLOAT3 m_eyePosition;
		float m_theta = 1.5f * DirectX::XM_PI;
		float m_phi = DirectX::XM_PIDIV4;
		float m_radius = 5.0f;

		Shader m_vertexShader;
		Shader m_pixelShader;

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
		std::unique_ptr<UploadBuffer<ConstantBufferTypes::ObjectConstants>> m_perObjectCB;
	};
}
