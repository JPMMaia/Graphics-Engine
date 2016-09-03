#pragma once

#include "Camera.h"
#include "D3DBase.h"
#include "MeshGeometry.h"
#include "Timer.h"
#include "MathHelper.h"
#include "Material.h"
#include "FrameResource.h"
#include "RenderItem.h"
#include "RenderLayer.h"
#include "Shader.h"

#include <vector>
#include "Texture.h"

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
		void LoadTextures();
		void InitializeRootSignature();
		void InitializeShadersAndInputLayout();
		void InitializeGeometry();
		void InitializeMaterials();
		void InitializeRenderItems();
		void InitializeFrameResources();
		void InitializePipelineStateObjects();

		void UpdateCamera();
		void UpdateObjectsConstantBuffer();
		void UpdateMaterialsConstantBuffer();
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
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_textureDescriptorHeap;
		std::unordered_map<std::string, Shader> m_shaders;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<ID3D12PipelineState>> m_pipelineStateObjects;

		ConstantBufferTypes::PassConstants m_passConstants;
		std::unique_ptr<UploadBuffer<ConstantBufferTypes::ObjectConstants>> m_perObjectCB;
		std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
		std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
		std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;

		std::vector<std::unique_ptr<RenderItem>> m_allRenderItems;
		std::vector<RenderItem*> m_renderItemLayers[static_cast<size_t>(RenderLayer::Count)];

		bool m_wireframeEnabled = false;
		Camera m_camera;
	};
}
