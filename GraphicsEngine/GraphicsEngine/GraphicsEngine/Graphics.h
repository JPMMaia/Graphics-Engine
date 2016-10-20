#pragma once

#include "Common/Timer.h"

#include "Camera.h"
#include "D3DBase.h"
#include "TextureManager.h"
#include "PipelineStateManager.h"
#include "RenderItem.h"
#include "RenderLayer.h"
#include "Scenes/DefaultScene.h"
#include "FrameResource.h"

namespace GraphicsEngine
{
	class Graphics
	{
	public:
		explicit Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight, bool fullscreen);

		void OnResize(uint32_t clientWidth, uint32_t clientHeight);
		void Update(const Common::Timer& timer);
		void Render(const Common::Timer& timer) const;

		Camera* GetCamera();

		void AddRenderItem(std::unique_ptr<RenderItem>&& renderItem, std::initializer_list<RenderLayer> renderLayers);

	private:
		void UpdateObjectsData();
		void UpdateMaterialData() const;
		void UpdatePassData(const Common::Timer& timer) const;

		void DrawRenderItems(RenderLayer renderItems) const;

	private:
		D3DBase m_d3dBase;
		PipelineStateManager m_pipelineStateManager;

		std::vector<std::unique_ptr<RenderItem>> m_allRenderItems;
		std::vector<RenderItem*> m_renderItemLayers[static_cast<SIZE_T>(RenderLayer::Count)];
		TextureManager m_textureManager;
		Camera m_camera;
		DefaultScene m_scene;

//		static const int s_frameResourcesCount = 3;
		std::vector<FrameResource> m_frameResources;
		FrameResource* m_currentFrameResource = nullptr;
		int m_currentFrameResourceIndex = 0;

//		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
//		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_postProcessRootSignature;

//		BufferTypes::PassData m_passConstants;
//		std::unique_ptr<UploadBuffer<BufferTypes::InstanceData>> m_perObjectCB;
	};
}
