#pragma once

#include <d3d11_2.h>
#include <string>
#include <wrl/client.h>

namespace GraphicsEngine
{
	class Texture
	{
	public:
		Texture();
		Texture(ID3D11Device* device, const std::wstring& filename);

		void Initialize(ID3D11Device* device, const std::wstring& filename);
		void Reset();

		ID3D11ShaderResourceView* Get() const;
		ID3D11ShaderResourceView* const* GetAddressOf() const;

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_textureView;
	};
}