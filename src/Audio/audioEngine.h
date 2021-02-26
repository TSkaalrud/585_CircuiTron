#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include "audioInstance.h"
#include <string>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <cstring>
#include <vector>
#include <memory>
#include <AL/al.h>
#include <AL/alc.h>
//#include "../../out/build/x64-Debug (default)/_deps/openal-src/include/AL/al.h"
//#include "../../out/build/x64-Debug (default)/_deps/openal-src/include/AL/alc.h"

namespace Audio {
	// audio is stored in an array, indexes are used to refer to the audio files in that array
	const int SOUND_FILE_THEME = 0;
	// const int SOUND_FILE_FINALCOUNT = 1;
	// const int SOUND_FILE_TTG_MAIN_MENU = 2;
	// const int SOUND_FILE_TTG_RACE = 3;

	#define NUM_BUFFERS 2
	#define NUM_SOURCES 2
	#define NUM_ENVIRONMENTS 2

	/*
	 * Struct that holds the RIFF data of the Wave file.
	 * The RIFF data is the meta data information that holds,
	 * the ID, size and format of the wave file
	 */
	struct RIFF_Header {
	char chunkID[4];
	int chunkSize; // size not including chunkSize or chunkID
	char format[4];
};

	/*
	 * Struct to hold fmt subchunk data for WAVE files.
	 */
	struct WAVE_Format {
	char subChunkID[4];
	int subChunkSize;
	short audioFormat;
	short numChannels;
	int sampleRate;
	int byteRate;
	short blockAlign;
	short bitsPerSample;
};

	/*
	 * Struct to hold the data of the wave file
	 */
	struct WAVE_Data {
	char subChunkID[4]; // should contain the word data
	int subChunk2Size;  // Stores the size of the data block
};

	class AudioEngine {
		public:
			AudioEngine();
			~AudioEngine();
			void initialize();
			void initializeBuffers();
			AudioInstance& createInstance(int soundFile);
			void killSources();

			ALfloat listenerPos[3]    = {0.0, 0.0, 4.0};
			ALfloat listenerVel[3]	  = {0.0, 0.0, 0.0};
			ALfloat listenerOri[6]	  = {0.0, 0.0, 1.0, 0.0, 1.0, 0.0};

			ALfloat source0Pos[3]	  = {-2.0, 0.0, 0.0};
			ALfloat source0Vel[3]	  = {0.0, 0.0, 0.0};

			ALuint buffer[NUM_BUFFERS];
			ALuint source[NUM_SOURCES];
			ALuint environment[NUM_ENVIRONMENTS];

			ALsizei size, freq;
			ALenum format;
			ALvoid* data;

		private:
			void CheckError(int op = -1, int _err = 0);
			bool _strcmp(const char* base, const char* cp);
			bool loadWavFile(const char* filename, ALuint* buffer, ALsizei* size, ALsizei* frequency, ALenum* format);

			static const int NUM_OF_SOUND_EFFECTS_POSSIBLE = 4;
			const char* soundFiles[NUM_OF_SOUND_EFFECTS_POSSIBLE];

			ALuint bufferArray[NUM_OF_SOUND_EFFECTS_POSSIBLE];
			std::vector<std::unique_ptr<AudioInstance>> listOfSources;

			ALCdevice* device;
			ALCcontext* context;
	};
} // namespace audioConstants