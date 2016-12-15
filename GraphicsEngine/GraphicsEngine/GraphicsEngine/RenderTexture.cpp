#include "stdafx.h"
#include "RenderTexture.h"

using namespace Common;
using namespace GraphicsEngine;

RenderTexture::RenderTexture(ID3D11Device* d3dDevice, UINT width, UINT height, DXGI_FORMAT format)
{
	// Create texture:
	{
		D3D11_TEXTURE2D_DESC description;
		description.Width = width;
		description.Height = height;
		description.MipLevels = 1;
		description.ArraySize = 1;
		description.Format = format;
		description.SampleDesc.Count = 1;
		description.SampleDesc.Quality = 0;
		description.Usage = D3D11_USAGE_DEFAULT;
		description.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		description.CPUAccessFlags = 0;
		description.MiscFlags = 0;

		ThrowIfFailed(d3dDevice->CreateTexture2D(&description, nullptr, m_texture.GetAddressOf()));
	}

	// Create shader resource view:
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC description;
		description.Format = format;
		description.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		description.Texture2D.MipLevels = 1;
		description.Texture2D.MostDetailedMip = 0;

		ThrowIfFailed(d3dDevice->CreateShaderResourceView(m_texture.Get(), &description, m_shaderResourceView.GetAddressOf()));
	}

	// Create render target view:
	{
		D3D11_RENDER_TARGET_VIEW_DESC description;
		description.Format = format;
		description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		description.Texture2D.MipSlice = 0;

		ThrowIfFailed(d3dDevice->CreateRenderTargetView(m_texture.Get(), &description, m_renderTargetView.GetAddressOf()));
	}
}

void RenderTexture::SetRenderTargetView(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView) const
{
	deviceContext->OMSetRenderTargets(1, m_renderTargetView.GetAddressOf(), depthStencilView);
}

void RenderTexture::ClearRenderTargetView(ID3D11DeviceContext* deviceContext, ID3D11DepthStencilView* depthStencilView, const DirectX::XMFLOAT4& color) const
{
	std::array<FLOAT, 4> clearColor = { color.x, color.y, color.z, color.w };

	deviceContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
	deviceContext->ClearRenderTargetView(m_renderTargetView.Get(), clearColor.data());
}

ID3D11ShaderResourceView* RenderTexture::GetShaderResourceView() const
{
	return m_shaderResourceView.Get();
}
