//***************************************************************************************
// Code extracted from d3dUtil.h by Frank Luna (C) 2015 All Rights Reserved.
//
// Geometry Helper
//***************************************************************************************

#pragma once

#include <DirectXCollision.h>
#include <unordered_map>

namespace GraphicsEngine
{
	/// <sumary>
	/// Defines a subrange of geometry in a MeshGeometry.  This is for when multiple
	/// geometries are stored in one vertex and index buffer.  It provides the offsets
	/// and data needed to draw a subset of geometry stores in the vertex and index 
	/// buffers so that we can implement the technique described by Figure 6.3.
	/// </sumary>
	struct SubmeshGeometry
	{
		UINT IndexCount = 0;
		UINT StartIndexLocation = 0;
		INT BaseVertexLocation = 0;

		/// <sumary>
		/// Bounding box of the geometry defined by this submesh. 
		/// This is used in later chapters of the book.
		/// </sumary>
		DirectX::BoundingBox Bounds;
	};

	struct MeshGeometry
	{
		// Give it a name so we can look it up by name.
		std::string Name;

		// System memory copies.  Use Blobs because the vertex/index format can be generic.
		// It is up to the client to cast appropriately.  
		Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

		Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
		Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

		UINT VertexByteStride = 0;
		UINT VertexBufferByteSize = 0;
		DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
		UINT IndexBufferByteSize = 0;

		/// <sumary>
		/// A MeshGeometry may store multiple geometries in one vertex/index buffer.
		/// Use this container to define the Submesh geometries so we can draw
		/// the Submeshes individually.
		/// </sumary>
		std::unordered_map<std::string, SubmeshGeometry> DrawArgs;

		D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

		D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

		/// <sumary>
		/// We can free this memory after we finish upload to the GPU.
		/// </sumary>
		void DisposeUploaders();
	};
}
