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
	//0 if available
	//SFX
	const int SOUND_FILE_AMBIENCE_BGM = 1;
	const int SOUND_FILE_APPREHENSION_SFX = 2;
	const int SOUND_FILE_BIKE_IMPACT_SFX = 3;
	const int SOUND_FILE_DESPAWN_SFX = 4;
	const int SOUND_FILE_ELECTRIC_HUM_SFX = 5;
	const int SOUND_FILE_ENGINE_IDLE_SFX = 6;
	const int SOUND_FILE_GUN_IMPACT_SFX = 7;
	const int SOUND_FILE_GUN_IMPACT2_SFX = 8;
	const int SOUND_FILE_HEALING_SFX = 9;
	const int SOUND_FILE_HEALING_2_SFX = 10;
	const int SOUND_FILE_HEALING_3_SFX = 11;
	const int SOUND_FILE_IDLE_WUB_SFX = 12;
	const int SOUND_FILE_REV_DOWN_SFX = 13;
	const int SOUND_FILE_REV_DOWN2_SFX = 14;
	const int SOUND_FILE_REV_STEADY_SFX = 15;
	const int SOUND_FILE_REV_UP_SFX = 16;
	const int SOUND_FILE_REV_UP2_SFX = 17;
	const int SOUND_FILE_REV_UP3_SFX = 18;
	const int SOUND_FILE_SIZZLE_SFX = 19;
	const int SOUND_FILE_SLIPSTREAM_SFX = 20;
	const int SOUND_FILE_SLIPSTREAM2_SFX = 21;
	const int SOUND_FILE_WAD_SFX = 22;
	const int SOUND_FILE_WUBWUB_SFX = 23;
	const int SOUND_FILE_BURR_SFX = 24;
	const int SOUND_FILE_BOOST_SFX = 25;
	//MUSIC
	const int SOUND_FILE_CYBERSONG_BGM = 101;
	const int SOUND_FILE_INITIAL_ENTRY_BGM = 102;
	const int SOUND_FILE_SINISTER_GALE_BGM = 103;
	const int SOUND_FILE_SOLAR_ECLIPSE_BGM = 104;
	const int SOUND_FILE_VICIOUS_CLARITY_BGM = 105;
	const int SOUND_FILE_AERIAL_WALKWAYS_BGM = 106;
	const int SOUND_FILE_AURORA_BGM = 107;



	#define NUM_BUFFERS 150
	#define NUM_SOURCES 50
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