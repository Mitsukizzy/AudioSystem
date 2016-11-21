#include "ITPEnginePCH.h"

AudioComponent::AudioComponent(Actor& owner)
{
	FMOD::System_Create(&system);

	PlayAudioData();
	Sound* skiesOpen = new Sound( "Assets/Sounds/Skyshot_SkiesOpen.wav" );
	mainChannel.Play( skiesOpen );
}

FMOD_RESULT F_CALLBACK WriteSoundData( FMOD_SOUND *sound, void *data, unsigned int datalen )
{
	// Cast to PCM and calculate sample count
	PCM16* pcmData = ( PCM16* ) data;
	// Number of samples that can fit in array when each sample is 2 bytes
	int pcmDataCount = datalen / 2;

	//mainChannel.WriteSoundData( pcmData, pcmDataCount );

	return FMOD_OK;
}

FMOD_RESULT AudioComponent::PlayAudioData()
{
	// Create and init sound info structure
	FMOD_CREATESOUNDEXINFO info;
	memset( &info, 0, sizeof( FMOD_CREATESOUNDEXINFO ) );
	info.cbsize = sizeof( FMOD_CREATESOUNDEXINFO );

	// 44100 Hz, Signed 16 bit format, 2 channels
	info.defaultfrequency = SAMPLE_RATE;
	info.format = FMOD_SOUND_FORMAT_PCM16;
	info.numchannels = 2;

	// 5 second duration
	info.length = 44100 * 2 * sizeof(signed short) * 5;
	info.decodebuffersize = SAMPLE_RATE * 0.1;	// Number of samples submitted every 100ms
	info.pcmreadcallback = WriteSoundData; //FMOD_SOUND_PCMREAD_CALLBACK

	// Create a looping stream with FMOD_OPENUSER and the info we filled
	FMOD::Sound* sound;
	FMOD_MODE mode = FMOD_LOOP_NORMAL | FMOD_OPENUSER;
	system->createStream( 0, mode, &info, &sound );
	//system->playSound( sound, NULL, false, 0 ); // 2nd param: Channel group defaults to FMOD_CHANNEL_FREE

	return FMOD_OK;
}
