#pragma once

#include <basetsd.h>
#include <String>
#include <rapidxml/rapidxml.hpp>

#ifdef GRAPHICS_ENGINE_EXPORT
#define GRAPHICS_ENGINE_API __declspec(dllexport)
#else
#define GRAPHICS_ENGINE_API
#endif

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
	void OutputAdaptersInfo(rapidxml::xml_document<wchar_t>* document, rapidxml::xml_node<wchar_t>* parent);
	void OutputAdapterInfo(rapidxml::xml_document<wchar_t>* document, rapidxml::xml_node<wchar_t>* parent, UINT adapterIndex, const DXGI_ADAPTER_DESC1& adapterDesc);

private:
	UINT m_adapterIndex;
	std::wstring m_adapterDescription;
	SIZE_T m_adapterDedicatedVideoMemory;
};