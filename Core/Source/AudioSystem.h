#pragma once
#include "Channel.h"
#include <fmod.hpp>
#include <fmod_errors.h>

#define SAMPLE_RATE 44100

class AudioSystem
{
public:
	AudioSystem();
	FMOD_RESULT PlayAudioData( const char* path );

private:
	static FMOD_RESULT F_CALLBACK WriteSoundDataCB( FMOD_SOUND *sound, void *data, unsigned int datalen );
	static FMOD_RESULT F_CALLBACK PCMSetPosCB( FMOD_SOUND *sound, int subsound, unsigned int position, FMOD_TIMEUNIT postype );
	FMOD_RESULT WriteSoundData( FMOD_SOUND *sound, void *data, unsigned int datalen );
	void ErrorCheck( FMOD_RESULT result );

	// Mono: count = sampling rate * duration * channels
	PCM16 data[SAMPLE_RATE];

	// Stereo: count = 44100 Hz * 1 second * 2 channels
	PCM16 stereoData[SAMPLE_RATE * 2];
	Channel mainChannel;
	FMOD::System* system;
};