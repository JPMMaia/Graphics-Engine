#include "stdafx.h"
#include "SettingsManager.h"
#include "DirectXHelper.h"

#include <fstream>
#include <rapidxml/rapidxml_print.hpp>

using namespace std;
using namespace rapidxml;
using namespace Microsoft::WRL;

SettingsManager::SettingsManager() :
	m_adapterIndex(0),
	m_adapterDescription(),
	m_adapterDedicatedVideoMemory(0)
{
}

SettingsManager SettingsManager::Build(const wstring& filename)
{
	SettingsManager settings;

	settings.CreateFileW(filename);

	return settings;
}

UINT SettingsManager::GetAdapterIndex() const
{
	return m_adapterIndex;
}

const wstring & SettingsManager::GetAdapterDescription() const
{
	return m_adapterDescription;
}

SIZE_T SettingsManager::GetAdapterDedicatedVideoMemory() const
{
	return m_adapterDedicatedVideoMemory;
}

void SettingsManager::CreateFile(const wstring& filename)
{
	// Create document:
	xml_document<wchar_t> document;

	// Create root node:
	xml_node<wchar_t>* settingsNode = document.allocate_node(node_type::node_element, L"Settings");
	document.append_node(settingsNode);

	// Output adapters info:
	AddAdaptersInfo(&document, settingsNode);

	// Create settings file:
	wofstream configFile(filename, ios::out);

	// Output document to file:
	rapidxml::print(ostream_iterator<wchar_t, wchar_t, char_traits<wchar_t>>(configFile), document);
}

void SettingsManager::AddAdaptersInfo(rapidxml::xml_document<wchar_t>* document, rapidxml::xml_node<wchar_t>* parent)
{
	// Create video cards node and append it to the settings node:
	xml_node<wchar_t>* videoCardsNode = document->allocate_node(node_type::node_element, L"VideoCards");
	parent->append_node(videoCardsNode);

	// Creates a DXGI 1.1 factory that can be used to generate other DXGI objects:
	ComPtr<IDXGIFactory1> factory;
	DX::ThrowIfFailed(CreateDXGIFactory1(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(factory.GetAddressOf())));

	// Enumerate through all the adapters, output info to file and select best adapter based on the Dedicated Video Memory:
	IDXGIAdapter1* adapter;
	for (auto adapterIndex = 0; factory->EnumAdapters1(adapterIndex, &adapter) != DXGI_ERROR_NOT_FOUND; adapterIndex++)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		adapter->GetDesc1(&adapterDesc);

		// Select best adapter based on the Dedicated Video Memory:
		if(adapterDesc.DedicatedVideoMemory > m_adapterDedicatedVideoMemory)
		{
			m_adapterIndex = adapterIndex;
			m_adapterDescription = adapterDesc.Description;
			m_adapterDedicatedVideoMemory = adapterDesc.DedicatedVideoMemory;
		}

		// Write adapter info to file:
		AddAdapterInfo(document, videoCardsNode, adapterIndex, adapterDesc);

		adapter->Release();
	}

	// Create default video card index node and add it to the video cards node:
	xml_node<wchar_t>* defaultVideoCardIndexNode = document->allocate_node(node_type::node_element, L"DefaultVideoCardIndex", AllocateValue(document, m_adapterIndex));
	videoCardsNode->append_node(defaultVideoCardIndexNode);
}

void SettingsManager::AddAdapterInfo(xml_document<wchar_t>* document, rapidxml::xml_node<wchar_t>* parent, UINT adapterIndex, const DXGI_ADAPTER_DESC1& adapterDesc) const
{
	// Create nodes to describe a video card:
	xml_node<wchar_t>* videoCardNode = document->allocate_node(node_type::node_element, L"VideoCard");
	xml_node<wchar_t>* indexNode = document->allocate_node(node_type::node_element, L"Index", AllocateValue(document, adapterIndex));
	xml_node<wchar_t>* descriptionNode = document->allocate_node(node_type::node_element, L"Description", document->allocate_string(adapterDesc.Description));
	xml_node<wchar_t>* dedicatedVideoMemoryNode = document->allocate_node(node_type::node_element, L"DedicatedVideoMemory", AllocateValue(document, adapterDesc.DedicatedVideoMemory));
	
	// Add video card node to parent node:
	parent->append_node(videoCardNode);

	// Add all others as children of the video card node:
	videoCardNode->append_node(indexNode);
	videoCardNode->append_node(descriptionNode);
	videoCardNode->append_node(dedicatedVideoMemoryNode);
}
