#pragma once
#include "Component.h"
#include <fmod.hpp>

typedef signed short PCM16;
typedef unsigned int U32;
typedef unsigned short U16;

class AudioComponent : public Component
{
	DECL_COMPONENT(AudioComponent, Component);
public:
	AudioComponent( Actor& owner );
	FMOD_RESULT F_CALLBACK WriteSoundData( FMOD_SOUND* sound, void* data, unsigned int length );

private:
	// Mono: count = sampling rate * duration * channels
	PCM16 data[44100];

	// Stereo: count = 44100 Hz * 1 second * 2 channels
	PCM16 stereoData[88200];
};

class MySound {
public:
	MySound(const char* path);
	~MySound();
	U32 samplingRate;
	U16 numChannels;
	U16 bitsPerSample;
	PCM16* data;
	U32 count;
};