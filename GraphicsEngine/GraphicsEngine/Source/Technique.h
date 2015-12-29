#pragma once

#include "ConstantBuffer.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "RasterizerState.h"

#include <map>

namespace GraphicsEngine
{
	class Technique
	{
	public:
		Technique();

		void Set(ID3D11DeviceContext1* d3dDeviceContext) const;

		void SetVertexShader(const VertexShader* vertexShader);
		void SetPixelShader(const PixelShader* pixelShader);
		void SetRasterizerState(const RasterizerState* rasterizerState);

		void SetVSConstantBuffer(ConstantBuffer* constantBuffer, uint32_t slot);
		void SetPSConstantBuffer(ConstantBuffer* constantBuffer, uint32_t slot);

	private:
		void SetConstantBuffers(ID3D11DeviceContext1* d3dDeviceContext) const;

	private:
		const VertexShader* m_vertexShader;
		const PixelShader* m_pixelShader;
		const RasterizerState* m_rasterizerState;

		std::map<uint32_t, ConstantBuffer*> m_vertexShaderConstantBuffers;
		std::map<uint32_t, ConstantBuffer*> m_pixelShaderConstantBuffers;
	};
}
