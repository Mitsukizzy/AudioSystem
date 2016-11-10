#include "ITPEnginePCH.h"
#include <SDL/SDL_mixer.h>
#include <fmod.hpp>
#include <iostream>
#include <fstream>

IMPL_COMPONENT(AudioComponent, Component);

AudioComponent::AudioComponent(Actor& owner)
	:Component(owner)
{
	// Create and init sound info structure
	FMOD_CREATESOUNDEXINFO info;
	memset( &info, 0, sizeof( FMOD_CREATESOUNDEXINFO ) );
	info.cbsize = sizeof( FMOD_CREATESOUNDEXINFO );

	// Specify sampling rate, format, and number of channels to use
	// In this case, 44100 Hz, signed 16-bit PCM, Stereo
	info.defaultfrequency = 44100;
	info.format = FMOD_SOUND_FORMAT_PCM16;
	info.numchannels = 2;

	// 5 second duration
	info.length = 44100 * 2 * sizeof(signed short) * 5;
	info.decodebuffersize = 4410;
	// Specify the callback function that will provide the audio data
	info.pcmreadcallback = WriteSoundData;

	// Create a looping stream with FMOD_OPENUSER and the info we filled
	//FMOD::Sound* sound;
	//FMOD_MODE mode = FMOD_LOOP_NORMAL | FMOD_OPENUSER;
	//system->createStream(0, mode, &info, &sound);
	//system->playSound(FMOD_CHANNEL_FREE, sound, false, 0);
}

FMOD_RESULT F_CALLBACK AudioComponent::WriteSoundData( FMOD_SOUND * sound, void * data, unsigned int length )
{
	// Cast data pointer to the appropriate format (in this case PCM16)
	PCM16* pcmData = (PCM16*)data;
	// Calculate how many samples can fit in the data array
	// In this case, since each sample has 2 bytes, we divide
	// length by 2
	int pcmDataCount = length / 2;
	// Output 0 in every sample
	for(int i = 0; i < pcmDataCount; ++i) {
		pcmData[i] = 0;
	}
	return FMOD_OK;
}

MySound::MySound(const char* path) {
	// Open file stream for input as binary
	std::ifstream file(path, std::ios::in | std::ios::binary);
	// Read number of channels and sample rate
	file.seekg(22);
	file.read((char*)&numChannels, 2);
	file.read((char*)&samplingRate, 4);
	// Read bits per sample
	file.seekg(34);
	file.read((char*)&bitsPerSample, 2);
	// Read size of data in bytes
	U32 length;
	file.seekg(40);
	file.read((char*)&length, 4);
	// Allocate array to hold all the data as PCM samples
	count = length / 2;
	data = new PCM16[count];
	// Read PCM data
	file.read((char*)data, length);
}

MySound::~MySound() {
	delete[] data;
}