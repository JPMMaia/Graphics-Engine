#pragma once

#include <d3d11_2.h>
#include <string>
#include <wrl/client.h>
#include <DirectXTex/DirectXTex/DirectXTex.h>

namespace GraphicsEngine
{
	class Texture
	{
	public:
		Texture() = default;
		Texture(ID3D11Device* device, const std::string& name, const std::wstring& filename);
		Texture(ID3D11Device* device, const std::string& name, ID3D11Resource* texture,  const D3D11_SHADER_RESOURCE_VIEW_DESC& srvDescription);

		void Initialize(ID3D11Device* device, const std::string& name, const std::wstring& filename);
		void Reset();

		ID3D11ShaderResourceView* Get() const;
		ID3D11ShaderResourceView* const* GetAddressOf() const;

	private:
		void CreateTextureFromFile(ID3D11Device* device, const std::wstring& filename);

	private:
		std::string m_name;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
	};
}
