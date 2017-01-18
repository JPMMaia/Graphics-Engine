#include "stdafx.h"
#include "DynamicCubeMapRenderTexture.h"
#include "Common/Helpers.h" 

using namespace Common;
using namespace GraphicsEngine;

DynamicCubeMapRenderTexture::DynamicCubeMapRenderTexture(ID3D11Device* d3dDevice, UINT width, UINT height, DXGI_FORMAT format)
{
	// Create texture:
	{
		D3D11_TEXTURE2D_DESC description;
		description.Width = width;
		description.Height = height;
		description.MipLevels = 0;
		description.ArraySize = 6;
		description.Format = format;
		description.SampleDesc.Count = 1;
		description.SampleDesc.Quality = 0;
		description.Usage = D3D11_USAGE_DEFAULT;
		description.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		description.CPUAccessFlags = 0;
		description.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		ThrowIfFailed(d3dDevice->CreateTexture2D(&description, nullptr, m_texture.GetAddressOf()));
	}

	// Create shader resource view:
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC description;
		description.Format = format;
		description.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		description.TextureCube.MipLevels = -1;
		description.TextureCube.MostDetailedMip = 0;

		ThrowIfFailed(d3dDevice->CreateShaderResourceView(m_texture.Get(), &description, m_shaderResourceView.GetAddressOf()));
	}

	// Create render target view:
	{
		D3D11_RENDER_TARGET_VIEW_DESC description;
		description.Format = format;
		description.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		description.Texture2DArray.ArraySize = 1;
		description.Texture2DArray.MipSlice = 0;
		
		for(size_t i = 0; i < m_renderTargetViews.size(); ++i)
		{
			description.Texture2DArray.FirstArraySlice = static_cast<UINT>(i);
			ThrowIfFailed(d3dDevice->CreateRenderTargetView(m_texture.Get(), &description, m_renderTargetViews[i].GetAddressOf()));
		}
	}

	// Create depth stencil texture:
	{
		D3D11_TEXTURE2D_DESC description;
		description.Width = width;
		description.Height = height;
		description.MipLevels = 0;
		description.ArraySize = 1;
		description.Format = DXGI_FORMAT_D32_FLOAT;
		description.SampleDesc.Count = 1;
		description.SampleDesc.Quality = 0;
		description.Usage = D3D11_USAGE_DEFAULT;
		description.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		description.CPUAccessFlags = 0;
		description.MiscFlags = 0;

		ThrowIfFailed(d3dDevice->CreateTexture2D(&description, nullptr, m_depthStencilTexture.GetAddressOf()));
	}

	// Create depth stencil view:
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC description;
		description.Format = DXGI_FORMAT_D32_FLOAT;
		description.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		description.Flags = 0;
		description.Texture2D.MipSlice = 0;

		ThrowIfFailed(d3dDevice->CreateDepthStencilView(m_depthStencilTexture.Get(), &description, m_depthStencilView.GetAddressOf()));
	}

	// Create viewport:
	{
		m_viewport.Width = static_cast<float>(width);
		m_viewport.Height = static_cast<float>(height);
		m_viewport.TopLeftX = 0.0f;
		m_viewport.TopLeftY = 0.0f;
		m_viewport.MinDepth = 0.0f;
		m_viewport.MaxDepth = 1.0f;
	}
}

void DynamicCubeMapRenderTexture::SetRenderTarget(ID3D11DeviceContext* deviceContext, UINT index) const
{
	deviceContext->OMSetRenderTargets(1, m_renderTargetViews[index].GetAddressOf(), m_depthStencilView.Get());
}
void DynamicCubeMapRenderTexture::ClearRenderTarget(ID3D11DeviceContext* deviceContext, UINT index) const
{
	static std::array<float, 4> clearColor = { 0.0f, 0.0f, 0.0f, 1.0f };
	deviceContext->ClearRenderTargetView(m_renderTargetViews[index].Get(), clearColor.data());
	deviceContext->ClearDepthStencilView(m_depthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0);
}
ID3D11ShaderResourceView* DynamicCubeMapRenderTexture::GetShaderResourceView() const
{
	return m_shaderResourceView.Get();
}