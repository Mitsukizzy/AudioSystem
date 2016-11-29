#include "ITPEnginePCH.h"

void Channel::Play( Sound* soundToPlay )
{
	sound = soundToPlay;
	position = 0;
}

void Channel::Stop()
{
	sound = 0;
}

void Channel::SetVolume( float newVolume )
{
	// 0 = silence, 1 = full volume
	volume = max( newVolume, 0.0f );
	volume = min( newVolume, 1.0f );
}

void Channel::WriteSoundData( PCM16* data, int count )
{
	if ( sound == 0 || paused )
		return;

	// Write samples to array
	// Increment in pairs because output is stereo
	for ( int i = 0; i < count; i += 2 )
	{
		if ( position >= sound->count )
		{
			if ( loop )
			{
				// Instead of stopping, set position back to beginning
				position = 0;
			}
			else
			{
				Stop();
				return;
			}
		}

		// Multiply value of audio data by volume before writing to output
		PCM16 curVal = ( PCM16 ) sound->data[position] * volume;

		// Channels for both left and right
		data[i] = curVal;
		data[i+1] = curVal;

		position++; // increments sample
	}
}