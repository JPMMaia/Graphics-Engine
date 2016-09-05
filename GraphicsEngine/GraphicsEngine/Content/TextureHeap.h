#pragma once

#include "Texture.h"

#include <unordered_map>

namespace GraphicsEngine
{
	class TextureHeap
	{
	public:
		void AddTexture(std::unique_ptr<Texture>&& texture);

		void Create(const D3DBase& d3dBase);

		ID3D12DescriptorHeap* GetDescriptorHeap() const;

	private:
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_textureDescriptorHeap;
		std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
	};
}
