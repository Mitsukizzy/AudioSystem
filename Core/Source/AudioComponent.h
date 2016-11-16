#pragma once
#include "Component.h"
#include "Channel.h"
#include <fmod.hpp>

class AudioComponent : public Component
{
	DECL_COMPONENT(AudioComponent, Component);
public:
	AudioComponent( Actor& owner );
	FMOD_RESULT PlayAudioData();
	FMOD_RESULT F_CALLBACK WriteSoundData( FMOD_SOUND* sound, void* data, unsigned int length );

private:
	// Mono: count = sampling rate * duration * channels
	PCM16 data[44100];

	// Stereo: count = 44100 Hz * 1 second * 2 channels
	PCM16 stereoData[88200];
	Channel mainChannel;
	FMOD::System* system;
};
