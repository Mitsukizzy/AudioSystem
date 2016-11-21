#pragma once
#include "Channel.h"
#include <fmod.hpp>

#define SAMPLE_RATE 44100

class AudioSystem
{
public:
	AudioSystem();
	FMOD_RESULT PlayAudioData();
	static FMOD_RESULT F_CALLBACK WriteSoundDataCB( FMOD_SOUND *sound, void *data, unsigned int datalen );
	FMOD_RESULT WriteSoundData( FMOD_SOUND *sound, void *data, unsigned int datalen );

private:
	// Mono: count = sampling rate * duration * channels
	PCM16 data[SAMPLE_RATE];

	// Stereo: count = 44100 Hz * 1 second * 2 channels
	PCM16 stereoData[SAMPLE_RATE * 2];
	Channel mainChannel;
	FMOD::System* system;
};