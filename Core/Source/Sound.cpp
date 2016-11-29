#include "ITPEnginePCH.h"
#include <iostream>
#include <fstream>

Sound::Sound( const char* path )
{
	// Open stream for binary input file
	std::ifstream file;
	file.open( path, std::ios::in | std::ios::binary );

	if ( !file )
	{
		std::cout << "FAILED TO PARSE AUDIO FILE" << std::endl;
	}
	DbgAssert( !file.fail(), "Path to Audio file is not valid" );

	// Read number of channels and sampling rate at offset 22 and 24 respectively
	file.seekg( 22 );
	file.read( ( char* ) &numChannels, 2 );
	file.read( ( char* ) &samplingRate, 4 );

	// Read bits per sample at offset 34
	file.seekg( 34 );
	file.read( ( char* ) &bitsPerSample, 2 );

	// Read data chunk size at offset 40
	file.seekg( 40 );
	file.read( ( char* ) &length, 4 );

	// Allocate array to hold all the data as PCM samples
	count = length / 2;
	data = new PCM16[count];

	// All data after past offset 44 is audio data, read into data variable
	file.read( ( char* ) data, length );
}

Sound::~Sound() 
{
	delete[] data;
}
