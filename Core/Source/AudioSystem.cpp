#include "ITPEnginePCH.h"
#include <iostream>

AudioSystem::AudioSystem()
{
	FMOD_RESULT result;
	int numDrivers;

	// System initialization with error checking
	result = FMOD::System_Create( &system );
	system->init( 50, FMOD_INIT_NORMAL, 0 );
	ErrorCheck( result );
	result = system->getNumDrivers( &numDrivers );
	ErrorCheck( result );
}

FMOD_RESULT AudioSystem::PlayAudioData( const char* path )
{
	Sound* sound1 = new Sound( path );
	mainChannel.Play( sound1 );	

	// Create and init sound info structure
	// Sets the WriteSoundData callback which uses the function defined in Channel
	FMOD_CREATESOUNDEXINFO info;
	memset( &info, 0, sizeof( FMOD_CREATESOUNDEXINFO ) );
	info.cbsize = sizeof( FMOD_CREATESOUNDEXINFO );

	// 44100 Hz, Signed 16 bit format, 2 channels
	info.defaultfrequency = SAMPLE_RATE;
	info.format = FMOD_SOUND_FORMAT_PCM16;
	info.numchannels = 2;
	info.length = sound1->length;	// length of the sound file
	info.decodebuffersize = 4410;	// Number of samples submitted every 100ms, Sample Rate (44100) * 0.1
	info.pcmreadcallback = &AudioSystem::WriteSoundDataCB; //FMOD_SOUND_PCMREAD_CALLBACK
	info.pcmsetposcallback = &AudioSystem::PCMSetPosCB;
	info.userdata = this;

	FMOD::Sound* sound;
	FMOD_MODE mode = FMOD_LOOP_NORMAL | FMOD_OPENMEMORY;
	FMOD_RESULT result;

	system->createStream( 0, FMOD_DEFAULT, &info, &sound ); 
	system->createSound( path, FMOD_DEFAULT, 0, &sound ); 
	system->playSound( sound, nullptr, false, 0 ); // 2nd param: Channel group defaults to FMOD_CHANNEL_FREE

	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK AudioSystem::WriteSoundDataCB( FMOD_SOUND *sound, void *data, unsigned int datalen )
{
	AudioSystem* as = new AudioSystem();
	return as->WriteSoundData( sound, data, datalen );	// trickery in order to keep this callback a static function
}

FMOD_RESULT AudioSystem::WriteSoundData( FMOD_SOUND *sound, void *data, unsigned int datalen )
{
	// Clear output
	memset( data, 0, datalen ); 
	// Cast to PCM and calculate sample count
	PCM16* pcmData = ( PCM16* ) data;
	// Number of samples that can fit in array when each sample is 2 bytes
	int pcmDataCount = datalen / 2;

	// Have the channel write to the output
	mainChannel.WriteSoundData( pcmData, pcmDataCount );

	return FMOD_OK;
}

FMOD_RESULT AudioSystem::PCMSetPosCB(FMOD_SOUND *sound, int subsound, unsigned int position, FMOD_TIMEUNIT postype)
{
	// Seek callback is required with read
	// Custom seek functionality is unneeded
	return FMOD_OK;
}

void AudioSystem::ErrorCheck(FMOD_RESULT result) 
{
	// Useful function for getting string explanations from FMOD errors
	if ( result != FMOD_OK )
	{
		const char* error = FMOD_ErrorString( result );
		std::cout << "FMOD Error: " << error << std::endl;
	}
}