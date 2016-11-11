#pragma once
#include "Actor.h"
#include "Sound.h"

// Encapsulates data and behaviors for playing sounds
class Channel : public Actor
{
	DECL_ACTOR(Channel, Actor);
public:
	Channel() : sound( 0 ), position( 0 ) {}
	void Play( Sound* soundToPlay );
	void Stop();
	void WriteSoundData( PCM16* data, int cout );

private:
	Sound* sound;
	int position;
};
DECL_PTR( Channel );