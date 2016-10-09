#pragma once

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
// ReSharper disable once CppUnusedIncludeDirective
#include <mmsystem.h>
#include <dsound.h>

#include <cstdint>
#include <unordered_map>
#include <wrl/client.h>

namespace SoundEngine
{
	/// <sumary>
	/// Implementation based on the Rastertek tutorial: "Direct Sound". http://www.rastertek.com/dx11tut14.html
	/// </sumary>
	class SoundManager
	{
	private:
		struct WaveHeaderType
		{
			uint8_t ChunkID[4];
			uint32_t ChunkSize;
			uint8_t Format[4];
			uint8_t SubChunkID[4];
			uint32_t SubChunkSize;
			uint16_t AudioFormat;
			uint16_t NumChannels;
			uint32_t SampleRate;
			uint32_t BytesPerSecond;
			uint16_t BlockAlign;
			uint16_t BitsPerSample;
			uint8_t DataChunkID[4];
			uint32_t DataSize;
		};

	public:
		SoundManager() = default;
		explicit SoundManager(HWND windowHandle);

		void Create2DSoundFromWaveFile(const std::string& name, const std::wstring& filename);
		void Play2DSound(const std::string& name, float volumePercentage);

	private:
		void InitializeDirectSound(HWND windowHandle);

		void LoadWaveFile(const std::wstring& filename, IDirectSoundBuffer8** secondaryBuffer) const;
		
	private:
		Microsoft::WRL::ComPtr<IDirectSound8> m_directSound;
		Microsoft::WRL::ComPtr<IDirectSoundBuffer> m_primaryBuffer;
		std::unordered_map<std::string, Microsoft::WRL::ComPtr<IDirectSoundBuffer8>> m_secondaryBuffers;
	};
}
