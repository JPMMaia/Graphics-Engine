#pragma once

#include "RenderItem.h"
#include "BillboardMeshGeometry.h"

namespace GraphicsEngine
{
	class BillboardRenderItem : public RenderItem
	{
	public:
		void Render(ID3D11DeviceContext* deviceContext) const override;
		void RenderNonInstanced(ID3D11DeviceContext* deviceContext) const override;

		void Update(ID3D11DeviceContext* deviceContext) const;

		void AddInstance(ID3D11Device* device, const BillboardMeshGeometry::VertexType& instance) const;
		void AddInstances(ID3D11Device* device, const std::vector<BillboardMeshGeometry::VertexType>& instances) const;
		void RemoveLastInstance() const;

		BillboardMeshGeometry* GetMesh() const;
		void SetMesh(BillboardMeshGeometry* mesh);

	private:
		BillboardMeshGeometry* m_mesh = nullptr;
	};
}
