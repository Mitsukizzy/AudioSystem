#pragma once

// Set some aliases for common audio formats
typedef signed short PCM16;
typedef unsigned int U32;
typedef unsigned short U16;

class Sound
{
public:
	Sound(const char* path);
	~Sound();
	U32 samplingRate;
	U16 numChannels;
	U16 bitsPerSample;
	PCM16* data;
	U32 length;
	U32 count;
};