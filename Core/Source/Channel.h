#pragma once
#include "Sound.h"

// Encapsulates data and behaviors for playing sounds
class Channel
{
public:
	Channel() : sound( 0 ), position( 0 ) {}
	void Play( Sound* soundToPlay );
	void Stop();
	void WriteSoundData( PCM16* data, int count );

	void SetPaused( bool isPaused ) { paused = isPaused; }
	bool GetPaused() const { return paused; }

	void SetLooping( bool isLooping ) { loop = isLooping; }
	bool GetLooping() const { return loop; }

	void SetVolume( float newVolume );
	float GetVolume() const { return volume; }

private:
	Sound* sound;
	uint16_t position;
	float volume;
	bool paused;
	bool loop;
};