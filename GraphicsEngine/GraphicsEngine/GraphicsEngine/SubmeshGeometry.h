//***************************************************************************************
// Code extracted from d3dUtil.h by Frank Luna (C) 2015 All Rights Reserved.
//
// Submesh Geometry
//***************************************************************************************

#pragma once

#include <DirectXCollision.h>

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
		uint32_t IndexCount = 0;
		uint32_t StartIndexLocation = 0;
		uint32_t BaseVertexLocation = 0;

		/// <sumary>
		/// Bounding box of the geometry defined by this submesh. 
		/// This is used in later chapters of the book.
		/// </sumary>
		DirectX::BoundingBox Bounds;
	};
}
