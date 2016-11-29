#include "ITPEnginePCH.h"
#include <iostream>

AudioSystem::AudioSystem()
{
	FMOD::System_Create(&system);
}

FMOD_RESULT AudioSystem::PlayAudioData( const char* path )
{
	FMOD_RESULT result;
	int numDrivers;
	FMOD_MODE mode = FMOD_OPENMEMORY | FMOD_LOOP_NORMAL;
	FMOD::Sound* sound;
	FMOD::Channel* musicChannel;

	// Error checking
	result = FMOD::System_Create( &system );
	ErrorCheck( result );
	result = system->getNumDrivers( &numDrivers );
	ErrorCheck( result );

	system->init( 50, FMOD_INIT_NORMAL, 0 );
	ErrorCheck( result );

	// Create and init sound info structure
	FMOD_CREATESOUNDEXINFO *info = new FMOD_CREATESOUNDEXINFO();
	//memset( info, 0, sizeof( FMOD_CREATESOUNDEXINFO ) );
	info->cbsize = sizeof( FMOD_CREATESOUNDEXINFO );

	// 44100 Hz, Signed 16 bit format, 2 channels
	info->defaultfrequency = SAMPLE_RATE;
	info->format = FMOD_SOUND_FORMAT_PCM16;
	info->numchannels = 2;

	// 5 second duration
	info->length = 44100 * 2 * sizeof(signed short) * 5;
	info->decodebuffersize = 4410;	// Number of samples submitted every 100ms, Sample Rate * 0.1
	info->pcmreadcallback = &AudioSystem::WriteSoundDataCB; //FMOD_SOUND_PCMREAD_CALLBACK
	info->pcmsetposcallback = &AudioSystem::PCMSetPosCB;
	info->userdata = this;

	result = system->createStream( 0, mode, info, &sound ); 
	ErrorCheck( result );
	result = system->playSound( sound, nullptr, false, &musicChannel ); // 2nd param: Channel group defaults to FMOD_CHANNEL_FREE
	ErrorCheck( result );

	//Sound* soundToPlay = new Sound( path );
	//mainChannel.Play( soundToPlay );
	

	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK AudioSystem::WriteSoundDataCB( FMOD_SOUND *sound, void *data, unsigned int datalen )
{
	AudioSystem* as = new AudioSystem();
	return as->WriteSoundData( sound, data, datalen );
}

FMOD_RESULT AudioSystem::WriteSoundData( FMOD_SOUND *sound, void *data, unsigned int datalen )
{
	memset( data, 0, datalen ); // clear output

	// Cast to PCM and calculate sample count
	PCM16* pcmData = ( PCM16* ) data;
	// Number of samples that can fit in array when each sample is 2 bytes
	int pcmDataCount = datalen / 2;

	mainChannel.WriteSoundData( pcmData, pcmDataCount );

	return FMOD_OK;
}

FMOD_RESULT AudioSystem::PCMSetPosCB(FMOD_SOUND *sound, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
	return FMOD_OK;
}


void AudioSystem::ErrorCheck(FMOD_RESULT result) 
{
	if ( result != FMOD_OK )
	{
		const char* error = FMOD_ErrorString( result );
		std::cout << "FMOD Error: " << error << std::endl;
	}
}