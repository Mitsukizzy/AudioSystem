#include "ITPEnginePCH.h"

IMPL_ACTOR(Channel, Actor);

void Channel::Play( Sound* soundToPlay )
{
	sound = soundToPlay;
	position = 0;
}

void Channel::Stop()
{
	sound = 0;
}

void Channel::WriteSoundData( PCM16* data, int count )
{
	if ( sound == 0 )
		return;

	// Write samples to array
	// Increment in pairs because output is stereo
	for ( int i = 0; i < count; i += 2 )
	{
		if ( position >= sound->count )
		{
			Stop();
			return;
		}

		PCM16 curVal = sound->data[position];

		// Channels for both left and right
		data[i] = curVal;
		data[i+1] = curVal;

		position++; // increments sample
	}
}