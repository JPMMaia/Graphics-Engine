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
#include "SamplerState.h"
#include "ShadowTexture.h"
#include "LightManager.h"
#include "RenderTexture.h"
#include "Octree.h"
#include "NormalRenderItem.h"

namespace GraphicsEngine
{
	class Graphics
	{
	public:
		explicit Graphics(HWND outputWindow, uint32_t clientWidth, uint32_t clientHeight, bool fullscreen);

		void OnResize(uint32_t clientWidth, uint32_t clientHeight);
		void FixedUpdate(const Common::Timer& timer);
		void RenderUpdate(const Common::Timer& timer);
		void Render(const Common::Timer& timer) const;

		Camera* GetCamera();
		DefaultScene* GetScene();

		void AddNormalRenderItem(std::unique_ptr<NormalRenderItem>&& renderItem, std::initializer_list<RenderLayer> renderLayers);
		void AddNormalRenderItemInstance(NormalRenderItem* renderItem, const ShaderBufferTypes::InstanceData& instanceData) const;
		uint32_t GetVisibleInstances() const;
		const std::vector<RenderItem*>& GetRenderItems(RenderLayer renderLayer) const;
		std::vector<std::unique_ptr<RenderItem>>::const_iterator GetRenderItem(std::string name) const;
		std::vector<NormalRenderItem*>::const_iterator GetNormalRenderItem(std::string name) const;

		void SetFogState(bool state);
		void SetFogDistanceParameters(float start, float range);
		void SetFogColor(const DirectX::XMFLOAT4& color);

	private:
		void BindSamplers() const;
		void SetupTerrainMeshData();

		void UpdateCamera();
		void UpdateInstancesDataFrustumCulling();
		void UpdateInstancesDataOctreeCulling();
		void UpdateMaterialData() const;
		void UpdateLights(const Common::Timer& timer) const;
		void InitializeMainPassData();
		void UpdateMainPassData(const Common::Timer& timer);
		void UpdateShadowPassData(const Common::Timer& timer) const;

		void DrawRenderItems(RenderLayer renderLayer) const;
		void DrawNonInstancedRenderItems(RenderLayer renderLayer) const;
		void DrawTerrain() const;

	private:
		bool m_initialized = false;
		D3DBase m_d3dBase;
		PipelineStateManager m_pipelineStateManager;

		std::vector<std::unique_ptr<RenderItem>> m_allRenderItems;
		std::vector<NormalRenderItem*> m_normalRenderItems;
		std::vector<RenderItem*> m_renderItemLayers[static_cast<SIZE_T>(RenderLayer::Count)];
		TextureManager m_textureManager;
		Camera m_camera;
		LightManager m_lightManager;
		Octree<OctreeCollider> m_octree;
		DefaultScene m_scene;

		std::vector<FrameResource> m_frameResources;
		FrameResource* m_currentFrameResource = nullptr;
		int m_currentFrameResourceIndex = 0;

		SamplerState m_linearClampSamplerState;
		SamplerState m_anisotropicWrapSamplerState;
		SamplerState m_anisotropicClampSamplerState;
		SamplerState m_shadowsSamplerState;
		bool m_fog;
		ShadowTexture m_shadowMap;
		RenderTexture m_renderTexture;
		DirectX::BoundingSphere m_sceneBounds;
		uint32_t m_visibleInstances;
		ShaderBufferTypes::PassData m_mainPassData;
	};
}
