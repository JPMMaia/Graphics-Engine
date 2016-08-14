#pragma once

#include "Camera.h"
#include "D3DBase.h"
#include "MeshGeometry.h"
#include "Timer.h"
#include "MathHelper.h"
#include "FrameResource.h"
#include "RenderItem.h"
#include "Shader.h"

#include <vector>

namespace GraphicsEngine
{
	class Graphics
	{
	public:
		explicit Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight);

		void OnResize(uint32_t clientWidth, uint32_t clientHeight);
		void Update(const Timer& timer);
		void Render(const Timer& timer);

		void SetWireframeMode(bool enable);

		static int GetFrameResourcesCount();
		Camera* GetCamera();
		
	private:
		void InitializeShadersAndInputLayout();
		void InitializeRootSignature();
		void InitializeDescriptorHeaps();
		void InitializeConstantBufferViews();
		void InitializePipelineStateObjects();
		void InitializeGeometry();
		void InitializeRenderItems();
		void InitializeFrameResources();

		void UpdateCamera();
		void UpdateObjectConstantBuffer();
		void UpdateMainPassConstantBuffer(const Timer& timer);

		void DrawRenderItems(ID3D12GraphicsCommandList* commandList, const std::vector<RenderItem*>& renderItems) const;

	private:
		D3DBase m_d3d;

		static const int s_frameResourcesCount = 3;
		std::vector<std::unique_ptr<FrameResource>> m_frameResources;
		FrameResource* m_currentFrameResource = nullptr;
		int m_currentFrameResourceIndex = 0;

		std::vector<std::unique_ptr<RenderItem>> m_allRenderItems;
		std::vector<RenderItem*> m_opaqueRenderItems;

		ConstantBufferTypes::PassConstants m_passConstants;

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_cbvHeap;
		uint32_t m_passCbvHeapOffset;
		std::unique_ptr<UploadBuffer<ConstantBufferTypes::ObjectConstants>> m_perObjectCB;

		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
		std::unordered_map<std::string, Shader> m_shaders;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_pipelineStateObjects;

		bool m_wireframeEnabled = false;
		Camera m_camera;
	};
}
