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
		IScene* GetScene();

		void AddRenderItem(std::unique_ptr<RenderItem>&& renderItem, std::initializer_list<RenderLayer> renderLayers);

	private:
		void UpdateInstancesDataFrustumCulling();
		void UpdateInstancesDataOctreeCulling();
		void UpdateMaterialData() const;
		void UpdateLights(const Common::Timer& timer) const;
		void UpdateMainPassData(const Common::Timer& timer) const;
		void UpdateShadowPassData(const Common::Timer& timer) const;

		void DrawRenderItems(RenderLayer renderLayer) const;
		void DrawNonInstancedRenderItems(RenderLayer renderLayer) const;
		void DrawTerrain() const;

	private:
		D3DBase m_d3dBase;
		PipelineStateManager m_pipelineStateManager;

		std::vector<std::unique_ptr<RenderItem>> m_allRenderItems;
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
		DirectX::XMFLOAT3 m_fogColor;
		ShadowTexture m_shadowMap;
		RenderTexture m_renderTexture;
		DirectX::BoundingSphere m_sceneBounds;		
	};
}
