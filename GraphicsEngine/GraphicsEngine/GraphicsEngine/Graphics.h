#pragma once

#include "Common/Timer.h"

#include "Camera.h"
#include "D3DBase.h"
#include "TextureManager.h"

namespace GraphicsEngine
{
	class Graphics
	{
	public:
		explicit Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight);

		void OnResize(uint32_t clientWidth, uint32_t clientHeight);
		void Update(const Common::Timer& timer);
		void Render(const Common::Timer& timer);

		Camera* GetCamera();

		//void AddRenderItem(std::unique_ptr<RenderItem>&& renderItem, std::initializer_list<RenderLayer> renderLayers);

	private:
		//void InitializeRootSignature();
		//void InitializeFrameResources();

		//void UpdateCamera();
		//void UpdateInstancesBuffer();
		//void UpdateMaterialsBuffer() const;
		//void UpdateMainPassBuffer(const Timer& timer);

		//void DrawRenderItems(ID3D12GraphicsCommandList* commandList, const std::vector<RenderItem*>& renderItems) const;

	private:
		D3DBase m_d3dBase;

//		static const int s_frameResourcesCount = 3;
//		std::vector<std::unique_ptr<FrameResource>> m_frameResources;
//		FrameResource* m_currentFrameResource = nullptr;
//		int m_currentFrameResourceIndex = 0;

//		PipelineStateManager m_pipelineStateManager;
//		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
//		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_postProcessRootSignature;

//		BufferTypes::PassData m_passConstants;
//		std::unique_ptr<UploadBuffer<BufferTypes::InstanceData>> m_perObjectCB;

//		std::vector<std::unique_ptr<RenderItem>> m_allRenderItems;
//		std::vector<RenderItem*> m_renderItemLayers[static_cast<size_t>(RenderLayer::Count)];

		Camera m_camera;
//		MirrorScene m_scene;
		TextureManager m_textureManager;
	};
}
