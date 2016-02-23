#pragma once

#include "BaseEffect.h"
#include "BufferTypes.h"
#include "ConstantBuffers.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "InstancedDataTypes.h"

namespace GraphicsEngine
{
	class ColorEffect : public BaseEffect<ColorEffect>
	{
	public:
		using InstancedDataType = InstancedDataTypes::World;
		using CameraBuffer = ConstantBuffers::CameraConstantBuffer;

	public:
		ColorEffect();
		explicit ColorEffect(ID3D11Device* d3dDevice);

		void Reset();

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const;

		void UpdateCameraBuffer(ID3D11DeviceContext1* d3dDeviceContext, const CameraBuffer& buffer) const;

	private:
		static const std::array<D3D11_INPUT_ELEMENT_DESC, 6> s_INPUT_ELEMENT_DESCRIPTION;

		VertexShader m_vertexShader;
		PixelShader m_pixelShader;
		DynamicConstantBuffer m_cameraBuffer;
	};
}
