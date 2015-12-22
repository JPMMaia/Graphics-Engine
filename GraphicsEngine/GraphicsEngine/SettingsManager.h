#pragma once

#include <basetsd.h>
#include <String>
#include <rapidxml/rapidxml.hpp>

class SettingsManager
{
public:
	static SettingsManager Build(const std::wstring& filename);

	UINT GetAdapterIndex() const;
	const std::wstring& GetAdapterDescription() const;
	SIZE_T GetAdapterDedicatedVideoMemory() const;

private:
	SettingsManager();

	void CreateFile(const std::wstring& filename);
	void AddAdaptersInfo(rapidxml::xml_document<wchar_t>* document, rapidxml::xml_node<wchar_t>* parent);
	void AddAdapterInfo(rapidxml::xml_document<wchar_t>* document, rapidxml::xml_node<wchar_t>* parent, UINT adapterIndex, const DXGI_ADAPTER_DESC1& adapterDesc) const;

	template<typename T>
	wchar_t* AllocateValue(rapidxml::xml_document<wchar_t>* document, T value) const;

private:
	UINT m_adapterIndex;
	std::wstring m_adapterDescription;
	SIZE_T m_adapterDedicatedVideoMemory;
};

template <typename T>
wchar_t* SettingsManager::AllocateValue(rapidxml::xml_document<wchar_t>* document, T value) const
{
	return document->allocate_string(std::to_wstring(value).data());
}