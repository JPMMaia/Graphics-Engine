#include "stdafx.h"
#include "SoundManager.h"
#include "EngineException.h"

#include <iostream>

using namespace GraphicsEngine;
using namespace std;

SoundManager::SoundManager(HWND windowHandle)
{
	InitializeDirectSound(windowHandle);
}

void SoundManager::Create2DSoundFromWaveFile(const string& name, const wstring& filename)
{
	// Check that the secondary buffer doesn't exist:
	if (m_secondaryBuffers.find(name) != m_secondaryBuffers.end())
		ThrowEngineException(L"A secondary buffer with the same name already exists.");

	// Create a new secondary buffer:
	auto& secondaryBuffer = m_secondaryBuffers[name];

	// Load secondary buffer using a wave file:
	LoadWaveFile(filename, secondaryBuffer.GetAddressOf());
}

void SoundManager::Play2DSound(const string& name, float volumePercentage)
{
	const auto& secondaryBuffer = m_secondaryBuffers.at(name);

	// Set position at the beginning of the sound buffer:
	DX::ThrowIfFailed(secondaryBuffer->SetCurrentPosition(0));

	// Set volume:
	{
		// Calculate volume. Map from range [0, 1] to [-10.000, 0]:
		auto volume = static_cast<LONG>(std::roundf((volumePercentage - 1.0f) * (-DSBVOLUME_MIN)));

		// Set volume of the buffer:
		DX::ThrowIfFailed(secondaryBuffer->SetVolume(volume));
	}

	// Play the contents of the secondary sound buffer:
	DX::ThrowIfFailed(secondaryBuffer->Play(0, 0, 0));
}

void SoundManager::InitializeDirectSound(HWND windowHandle)
{
	// Create Direct Sound:
	DX::ThrowIfFailed(DirectSoundCreate8(nullptr, m_directSound.GetAddressOf(), nullptr));

	// Set the cooperative level to priority so the format of the primary sound buffer can be modified:
	DX::ThrowIfFailed(m_directSound->SetCooperativeLevel(windowHandle, DSSCL_PRIORITY));

	DSBUFFERDESC bufferDescription;
	bufferDescription.dwSize = sizeof(DSBUFFERDESC);
	bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	bufferDescription.dwBufferBytes = 0;
	bufferDescription.dwReserved = 0;
	bufferDescription.lpwfxFormat = nullptr;
	bufferDescription.guid3DAlgorithm = GUID_NULL;

	// Get control of the primary sound buffer on the default sound device:
	DX::ThrowIfFailed(m_directSound->CreateSoundBuffer(&bufferDescription, m_primaryBuffer.GetAddressOf(), nullptr));

	// Setup the format of the primary sound bufffer.
	// In this case it is a .WAV file recorded at 44,100 samples per second in 16-bit stereo (cd audio format):
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	// Set the primary buffer to be the wave format specified:
	DX::ThrowIfFailed(m_primaryBuffer->SetFormat(&waveFormat));
}

void SoundManager::LoadWaveFile(const std::wstring& filename, IDirectSoundBuffer8** secondaryBuffer) const
{
	// Open the wave file in binary mode:
	ifstream fileStream(filename, ios::in | ios::binary);

	// Read header:
	WaveHeaderType waveFileHeader;
	fileStream.read(reinterpret_cast<char*>(&waveFileHeader), sizeof(WaveHeaderType));

	// Check for errors in the file:
	{
		// Check that the chunk ID is the RIFF format:
		if ((waveFileHeader.ChunkID[0] != 'R') || (waveFileHeader.ChunkID[1] != 'I') ||
			(waveFileHeader.ChunkID[2] != 'F') || (waveFileHeader.ChunkID[3] != 'F'))
			ThrowEngineException(L"Chunk ID isn't the 'RIFF' format.");

		// Check that the file format is the WAVE format:
		if ((waveFileHeader.Format[0] != 'W') || (waveFileHeader.Format[1] != 'A') ||
			(waveFileHeader.Format[2] != 'V') || (waveFileHeader.Format[3] != 'E'))
			ThrowEngineException(L"File format isn't the 'WAVE' format.");

		// Check that the sub chunk ID is the fmt format:
		if ((waveFileHeader.SubChunkID[0] != 'f') || (waveFileHeader.SubChunkID[1] != 'm') ||
			(waveFileHeader.SubChunkID[2] != 't') || (waveFileHeader.SubChunkID[3] != ' '))
			ThrowEngineException(L"Sub Chunk ID isn't the 'fmt' format.");

		// Check that the audio format is WAVE_FORMAT_PCM:
		if (waveFileHeader.AudioFormat != WAVE_FORMAT_PCM)
			ThrowEngineException(L"Audio format isn't the 'WAVE_FORMAT_PCM' format.");

		// Check that the wave file was recorded in stereo format:
		if (waveFileHeader.NumChannels != 2)
			ThrowEngineException(L"Number of channels must be equal to 2.");

		// Check that the wave file was recorded at a sample rate of 44.1 KHz:
		if (waveFileHeader.SampleRate != 44100)
			ThrowEngineException(L"Sample rate must be equal to 44.1 KHz.");

		// Ensure that the wave file was recorded in 16 bit format:
		if (waveFileHeader.BitsPerSample != 16)
			ThrowEngineException(L"Bits per Sample must be equal to 16.");

		// Check for the data chunk header:
		if ((waveFileHeader.DataChunkID[0] != 'd') || (waveFileHeader.DataChunkID[1] != 'a') ||
			(waveFileHeader.DataChunkID[2] != 't') || (waveFileHeader.DataChunkID[3] != 'a'))
			ThrowEngineException(L"Data chunk not found!");
	}

	// Create secondary buffer:
	{
		// Set the wave format of secondary buffer that this wave file will be loaded onto:
		WAVEFORMATEX waveFormat;
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = 44100;
		waveFormat.wBitsPerSample = 16;
		waveFormat.nChannels = 2;
		waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;

		// Set the buffer description of the secondary sound buffer that the wave file will be loaded onto:
		DSBUFFERDESC bufferDescription;
		bufferDescription.dwSize = sizeof(DSBUFFERDESC);
		bufferDescription.dwFlags = DSBCAPS_CTRLVOLUME;
		bufferDescription.dwBufferBytes = waveFileHeader.DataSize;
		bufferDescription.dwReserved = 0;
		bufferDescription.lpwfxFormat = &waveFormat;
		bufferDescription.guid3DAlgorithm = GUID_NULL;

		// Create a temporary sound buffer with the specific buffer settings:
		Microsoft::WRL::ComPtr<IDirectSoundBuffer> temporaryBuffer;
		DX::ThrowIfFailed(m_directSound->CreateSoundBuffer(&bufferDescription, temporaryBuffer.GetAddressOf(), nullptr));

		// Test the buffer format against the direct sound 8 interface and create the secondary buffer:
		DX::ThrowIfFailed(temporaryBuffer->QueryInterface(IID_IDirectSoundBuffer8, reinterpret_cast<void**>(&*secondaryBuffer)));
	}

	// Create a temporary buffer to hold the wave file data:
	vector<uint8_t> waveData(waveFileHeader.DataSize);
	{
		// Move to the beginning of the wave data which starts at the end of the data chunk header:
		fileStream.seekg(sizeof(WaveHeaderType), ios::beg);

		// Read in the wave file data into the newly created buffer:
		auto count = fileStream.readsome(reinterpret_cast<char*>(&waveData[0]), waveFileHeader.DataSize);
		if (count != waveFileHeader.DataSize)
			ThrowEngineException(L"Number of read bytes doesn't match data size.");
	}

	// Close the file once done reading:
	fileStream.close();

	// Copy data from waveData to secondary buffer:
	{
		// Lock the secondary buffer to write wave data into it:
		uint8_t* pBuffer;
		DWORD bufferSize;
		DX::ThrowIfFailed((*secondaryBuffer)->Lock(0, waveFileHeader.DataSize, reinterpret_cast<void**>(&pBuffer), &bufferSize, nullptr, nullptr, 0));

		// Copy the wave data into the buffer:
		CopyMemory(pBuffer, &waveData[0], waveFileHeader.DataSize);

		// Unlock the secondary buffer after the data has been written to it:
		DX::ThrowIfFailed((*secondaryBuffer)->Unlock(static_cast<void*>(pBuffer), bufferSize, nullptr, 0));
	}
}
