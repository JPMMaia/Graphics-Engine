#pragma once

#include "RenderItem.h"
#include "ImmutableMeshGeometry.h"
#include "CubeMappingCamera.h"
#include "CubeMapRenderTexture.h"
#include "ShaderBufferTypes.h"

namespace GraphicsEngine
{
	class CubeMappingRenderItem : public RenderItem
	{
	public:
		explicit CubeMappingRenderItem(ID3D11Device* device, ImmutableMeshGeometry* mesh, const std::string& submeshName);

		void Render(ID3D11DeviceContext* deviceContext) const override;
		void RenderNonInstanced(ID3D11DeviceContext* deviceContext) const override;
		void RemoveLastInstance() override;

		void SetPosition(DirectX::FXMVECTOR position);

		const CubeMappingCamera& GetCamera() const;
		const CubeMapRenderTexture& GetRenderTexture() const;
		DirectX::XMVECTOR  GetPosition() const;
		const ShaderBufferTypes::InstanceData& GetInstanceData() const;
		const SubmeshGeometry& GetSubmesh() const;

	private:
		ImmutableMeshGeometry* m_mesh;
		std::string m_submeshName;
		CubeMappingCamera m_camera;
		CubeMapRenderTexture m_renderTexture;

		DirectX::XMVECTOR m_position;
		ShaderBufferTypes::InstanceData m_instanceData;
		InstanceBuffer m_instanceBuffer;
	};
}
