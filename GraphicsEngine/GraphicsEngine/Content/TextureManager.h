#pragma once
#include <unordered_map>

namespace GraphicsEngine
{
	class DescriptorHeap;
	class D3DBase;
	struct Texture;

	class TextureManager
	{
	public:
		void AddTexture(std::unique_ptr<Texture>&& texture);

		void LoadAllTextures(const D3DBase& d3dBase);
		void CreateShaderResourceViews(const D3DBase& d3dBase, DescriptorHeap* descriptorHeap);

		Texture* GetTexture(const std::string& name) const;
		size_t GetTextureCount() const;

	private:
		std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;
	};
}
