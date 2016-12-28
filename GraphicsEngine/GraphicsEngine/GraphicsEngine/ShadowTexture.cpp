#include "stdafx.h"
#include "ShadowTexture.h"

using namespace Common;
using namespace GraphicsEngine;

ShadowTexture::ShadowTexture(ID3D11Device* d3dDevice, UINT width, UINT height) :
	m_width(width),
	m_height(height)
{
	// Create texture:
	{
		D3D11_TEXTURE2D_DESC description;
		description.Width = width;
		description.Height = height;
		description.MipLevels = 1;
		description.ArraySize = 1;
		description.Format = DXGI_FORMAT_R32_TYPELESS;
		description.SampleDesc.Count = 1;
		description.SampleDesc.Quality = 0;
		description.Usage = D3D11_USAGE_DEFAULT;
		description.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		description.CPUAccessFlags = 0;
		description.MiscFlags = 0;

		ThrowIfFailed(d3dDevice->CreateTexture2D(&description, nullptr, m_texture.GetAddressOf()));
	}

	// Create shader resource view:
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC description;
		description.Format = DXGI_FORMAT_R32_FLOAT;
		description.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		description.Texture2D.MipLevels = 1;
		description.Texture2D.MostDetailedMip = 0;

		ThrowIfFailed(d3dDevice->CreateShaderResourceView(m_texture.Get(), &description, m_shaderResourceView.GetAddressOf()));
	}

	// Create depth stencil view:
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC description;
		description.Format = DXGI_FORMAT_D32_FLOAT;
		description.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		description.Flags = 0;
		description.Texture2D.MipSlice = 0;

		ThrowIfFailed(d3dDevice->CreateDepthStencilView(m_texture.Get(), &description, m_depthStencilView.GetAddressOf()));
	}
}

void ShadowTexture::SetDepthStencilView(ID3D11DeviceContext* deviceContext) const
{
	deviceContext->OMSetRenderTargets(0, nullptr, m_depthStencilView.Get());
}
void ShadowTexture::ClearDepthStencilView(ID3D11DeviceContext* deviceContext) const
{
	deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}
UINT ShadowTexture::GetWidth() const
{
	return m_width;
}
UINT ShadowTexture::GetHeight() const
{
	return m_height;
}

ID3D11Texture2D* ShadowTexture::GetTexture() const
{
	return m_texture.Get();
}
ID3D11ShaderResourceView* ShadowTexture::GetShaderResourceView() const
{
	return m_shaderResourceView.Get();
}
