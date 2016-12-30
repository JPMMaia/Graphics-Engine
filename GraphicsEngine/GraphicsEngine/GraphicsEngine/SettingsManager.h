#pragma once

#include <basetsd.h>
#include <string>
#include <rapidxml/rapidxml.hpp>
#include <dxgi1_2.h>

namespace GraphicsEngine
{
	class SettingsManager
	{
	public:
		static SettingsManager Build(const std::wstring& filename);

		uint32_t GetAdapterIndex() const;
		const std::wstring& GetAdapterDescription() const;
		SIZE_T GetAdapterDedicatedVideoMemory() const;

	private:
		SettingsManager();

		void ReadFile(const std::wstring& filename);

		void CreateFile(const std::wstring& filename);
		void AddAdaptersInfo(rapidxml::xml_document<wchar_t>* document, rapidxml::xml_node<wchar_t>* parent);
		void AddAdapterInfo(rapidxml::xml_document<wchar_t>* document, rapidxml::xml_node<wchar_t>* parent, uint32_t adapterIndex, const DXGI_ADAPTER_DESC2& adapterDesc) const;

		template<typename T>
		wchar_t* AllocateValue(rapidxml::xml_document<wchar_t>* document, T value) const;

	private:
		uint32_t m_adapterIndex;
		std::wstring m_adapterDescription;
		SIZE_T m_adapterDedicatedVideoMemory;
	};

	template <typename T>
	wchar_t* SettingsManager::AllocateValue(rapidxml::xml_document<wchar_t>* document, T value) const
	{
		return document->allocate_string(std::to_wstring(value).data());
	}
}