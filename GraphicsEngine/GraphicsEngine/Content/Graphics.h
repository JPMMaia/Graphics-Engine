#pragma once

#include "BlurFilter.h"
#include "Camera.h"
#include "D3DBase.h"
#include "MeshGeometry.h"
#include "Timer.h"
#include "MathHelper.h"
#include "FrameResource.h"
#include "PipelineStateManager.h"
#include "RenderItem.h"
#include "RenderLayer.h"
#include "TextureManager.h"
#include "DescriptorHeap.h"
#include "Scenes/MirrorScene.h"


#include <vector>

namespace GraphicsEngine
{
	class Graphics
	{
	public:
		static int GetFrameResourcesCount();

	public:
		explicit Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight);

		void OnResize(uint32_t clientWidth, uint32_t clientHeight);
		void Update(const Timer& timer);
		void Render(const Timer& timer);

		void SetWireframeMode(bool enable);

		Camera* GetCamera();

		void AddRenderItem(std::unique_ptr<RenderItem>&& renderItem, std::initializer_list<RenderLayer> renderLayers);
		
	private:
		void InitializeRootSignature();
		void InitializeFrameResources();

		void UpdateCamera();
		void UpdateObjectsConstantBuffer();
		void UpdateMaterialsConstantBuffer() const;
		void UpdateMainPassConstantBuffer(const Timer& timer);

		void DrawRenderItems(ID3D12GraphicsCommandList* commandList, const std::vector<RenderItem*>& renderItems) const;

	private:
		D3DBase m_d3d;

		static const int s_frameResourcesCount = 3;
		std::vector<std::unique_ptr<FrameResource>> m_frameResources;
		FrameResource* m_currentFrameResource = nullptr;
		int m_currentFrameResourceIndex = 0;

		PipelineStateManager m_pipelineStateManager;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_postProcessRootSignature;

		ConstantBufferTypes::PassConstants m_passConstants;
		std::unique_ptr<UploadBuffer<ConstantBufferTypes::ObjectConstants>> m_perObjectCB;

		std::vector<std::unique_ptr<RenderItem>> m_allRenderItems;
		std::vector<RenderItem*> m_renderItemLayers[static_cast<size_t>(RenderLayer::Count)];

		bool m_wireframeEnabled = false;
		Camera m_camera;
		MirrorScene m_scene;
		TextureManager m_textureManager;
		DescriptorHeap m_descriptorHeap;
		BlurFilter m_blurFilter;
	};
}
