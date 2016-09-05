#pragma once

#include "Camera.h"
#include "D3DBase.h"
#include "MeshGeometry.h"
#include "Timer.h"
#include "MathHelper.h"
#include "FrameResource.h"
#include "RenderItem.h"
#include "RenderLayer.h"
#include "Shader.h"

#include <vector>
#include "Scenes/DefaultScene.h"
#include "TextureHeap.h"

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

		void AddTexture(std::unique_ptr<Texture>&& texture);
		void AddRenderItem(std::unique_ptr<RenderItem>&& renderItem, RenderLayer renderLayer);
		
	private:
		void InitializeRootSignature();
		void InitializeShadersAndInputLayout();
		void InitializeFrameResources();
		void InitializePipelineStateObjects();

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

		std::vector<D3D12_INPUT_ELEMENT_DESC> m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		std::unordered_map<std::string, Shader> m_shaders;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_pipelineStateObjects;

		ConstantBufferTypes::PassConstants m_passConstants;
		std::unique_ptr<UploadBuffer<ConstantBufferTypes::ObjectConstants>> m_perObjectCB;

		std::vector<std::unique_ptr<RenderItem>> m_allRenderItems;
		std::vector<RenderItem*> m_renderItemLayers[static_cast<size_t>(RenderLayer::Count)];

		bool m_wireframeEnabled = false;
		Camera m_camera;
		DefaultScene m_scene;
		TextureHeap m_textureHeap;
	};
}
