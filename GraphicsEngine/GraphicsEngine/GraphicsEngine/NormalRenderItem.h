#pragma once

#include "RenderItem.h"
#include "ShaderBufferTypes.h"
#include "OctreeCollider.h"

#include <unordered_set>

namespace GraphicsEngine
{
	class ImmutableMeshGeometry;
	struct SubmeshGeometry;

	class NormalRenderItem : public RenderItem
	{
	public:
		NormalRenderItem() = default;
		
		void Render(ID3D11DeviceContext* deviceContext) const override;
		void RenderNonInstanced(ID3D11DeviceContext* deviceContext) const override;

		void AddInstance(const ShaderBufferTypes::InstanceData& instanceData);
		void SetInstance(size_t instanceID, const ShaderBufferTypes::InstanceData& instanceData);
		const ShaderBufferTypes::InstanceData& GetInstance(size_t instanceID);
		void RemoveLastInstance();
		void InscreaseInstancesCapacity(size_t aditionalCapacity);

		void InsertVisibleInstance(size_t instanceID);
		void ClearVisibleInstances();

		ImmutableMeshGeometry* GetMesh() const;
		void SetMesh(ImmutableMeshGeometry* mesh, const std::string& submeshName);
		const SubmeshGeometry& GetSubmesh() const;
		const std::vector<ShaderBufferTypes::InstanceData>& GetInstancesData() const;
		const std::vector<OctreeCollider>& GetColliders() const;
		std::vector<OctreeCollider>& GetColliders();
		size_t GetVisibleInstanceCount() const;
		void SetVisibleInstanceCount(size_t visibleInstanceCount);
		const std::unordered_set<uint32_t>& GetVisibleInstances() const;
		
	private:
		void SetInputAssemblerData(ID3D11DeviceContext* deviceContext) const;

	private:
		ImmutableMeshGeometry* m_mesh;
		std::string m_submeshName;
		size_t m_visibleInstanceCount = 0;
		std::vector<ShaderBufferTypes::InstanceData> m_instancesData;
		std::vector<OctreeCollider> m_colliders;
		std::unordered_set<uint32_t> m_visibleInstances;
	};
}
