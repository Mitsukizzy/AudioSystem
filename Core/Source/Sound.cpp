#include "ITPEnginePCH.h"
#include <iostream>
#include <fstream>

Sound::Sound( const char* path )
{
	// Open stream for binary input file
	std::ifstream file( path, std::ios::in | std::ios::binary ); 

	if ( !file )
	{
		std::cout << "FAILED TO PARSE AUDIO FILE" << std::endl;
	}
	DbgAssert( !file.fail(), "Path to Audio file is not valid" );

	// Read number of channels and sampling rate at offset 22 and 24 respectively
	file.seekg( 22 );
	file.read( ( char* ) &numChannels, 2 );
	file.read( ( char* ) &samplingRate, 4 );

	// Read bits per sample
	file.seekg( 34 );
	file.read( ( char* ) &bitsPerSample, 2 );

	// Read size of data in bytes
	U32 length;
	file.seekg( 40 );
	file.read( ( char* ) &length, 4 );

	// Allocate array to hold all the data as PCM samples
	count = length / 2;
	data = new PCM16[count];

	// Read PCM data into data
	file.read( ( char* ) data, length );
}

Sound::~Sound() 
{
	delete[] data;
}
