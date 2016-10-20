#pragma once

#include "Buffer.h"

namespace GraphicsEngine
{
	using DefaultConstantBuffer = Buffer<D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DEFAULT, 0>;
	using DynamicConstantBuffer = Buffer<D3D11_BIND_CONSTANT_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE>;
	using IndexBuffer = Buffer<D3D11_BIND_INDEX_BUFFER, D3D11_USAGE_IMMUTABLE, 0>;
	using InstanceBuffer = Buffer<D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_DYNAMIC, D3D11_CPU_ACCESS_WRITE>;
	using VertexBuffer = Buffer<D3D11_BIND_VERTEX_BUFFER, D3D11_USAGE_IMMUTABLE, 0>;
}