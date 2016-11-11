#pragma once
#include "Sound.h"

// Encapsulates data and behaviors for playing sounds
class Channel
{
public:
	Channel() : sound( 0 ), position( 0 ) {}
	void Play( Sound* soundToPlay );
	void Stop();
	void WriteSoundData( PCM16* data, int cout );

private:
	Sound* sound;
	int position;
};