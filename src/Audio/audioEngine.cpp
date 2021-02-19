#include "audioEngine.h"

namespace Audio {

AudioEngine::AudioEngine() {

}
AudioEngine::~AudioEngine() {};
/*
 * Load wave file function. No need for ALUT with this
 */

void AudioEngine::CheckError(int op, int _err) {
	int err;
	if (op == -1)
		err = alGetError(); // clear any error messages
	else
		err = _err;
	if (err != AL_NO_ERROR) {
		if (err == AL_INVALID_NAME)
			fprintf(stderr, "Error : Invalid Name\n");
		else if (err == AL_INVALID_ENUM)
			fprintf(stderr, "Error : Invalid Enum\n");
		else if (err == AL_INVALID_VALUE)
			fprintf(stderr, "Error : Invalid Value\n");
		else if (err == AL_INVALID_OPERATION)
			fprintf(stderr, "Error : Invalid Operation\n");
		else if (err == AL_OUT_OF_MEMORY)
			fprintf(stderr, "Error : Out of Memory\n");
		exit(1);
	}
	return;
}

bool AudioEngine::_strcmp(const char* base, const char* cp) {
	for (int lx = 0; base[lx] != 0; lx++) {
		if (cp[lx] != base[lx])
			return false;
	}
	return true;
}

bool AudioEngine::loadWavFile(const char* filename, ALuint* buffer, ALsizei* size, ALsizei* frequency, ALenum* format) {
	// Local Declarations
	fprintf(stderr, "[1] filename = %s\n", filename);
	FILE* soundFile = NULL;
	WAVE_Format wave_format;
	RIFF_Header riff_header;
	WAVE_Data wave_data;
	unsigned char* data = 0;

	try {
		soundFile = fopen(filename, "rb");
		if (!soundFile)
			throw(filename);

		// Read in the first chunk into the struct
		fread(&riff_header, sizeof(RIFF_Header), 1, soundFile);

		// check for RIFF and WAVE tag in memeory
		if (_strcmp("RIFF", riff_header.chunkID) == false or _strcmp("WAVE", riff_header.format) == false)
			throw("Invalid RIFF or WAVE Header");

		// Read in the 2nd chunk for the wave info
		fread(&wave_format, sizeof(WAVE_Format), 1, soundFile);
		// check for fmt tag in memory
		if (_strcmp("fmt ", wave_format.subChunkID) == false)
			throw("Invalid Wave Format");
		
		// check for extra parameters;
		if (wave_format.subChunkSize > 16)
			fseek(soundFile, sizeof(short), SEEK_CUR);

		// Read in the the last byte of data before the sound file
		fread(&wave_data, sizeof(WAVE_Data), 1, soundFile);
		// check for data tag in memory
		if (_strcmp("data", wave_data.subChunkID) == false)
			throw("Invalid data header");

		// Allocate memory for data
		data = new unsigned char[wave_data.subChunk2Size];

		// Read in the sound data into the soundData variable
		if (!fread(data, wave_data.subChunk2Size, 1, soundFile))
			throw("error loading WAVE data into struct!");

		// Now we set the variables that we passed in with the
		// data from the structs
		*size = wave_data.subChunk2Size;
		*frequency = wave_format.sampleRate;
		// The format is worked out by looking at the number of
		// channels and the bits per sample.
		if (wave_format.numChannels == 1) {
			if (wave_format.bitsPerSample == 8)
				*format = AL_FORMAT_MONO8;
			else if (wave_format.bitsPerSample == 16)
				*format = AL_FORMAT_MONO16;
		} else if (wave_format.numChannels == 2) {
			if (wave_format.bitsPerSample == 8)
				*format = AL_FORMAT_STEREO8;
			else if (wave_format.bitsPerSample == 16)
				*format = AL_FORMAT_STEREO16;
		}
		// create our openAL buffer and check for success
		CheckError();
		alGenBuffers(1, buffer);
		CheckError();
		// now we put our data into the openAL buffer and
		// check for success
		alBufferData(*buffer, *format, (void*)data, *size, *frequency);
		//CheckError();
		// clean up and return true if successful
		fclose(soundFile);
		fprintf(stderr, "load ok\n");
		delete[] data;
		return true;
	} catch (std::string error) {
		// our catch statement for if we throw a string
		std::cout << error << std::endl;
		// clean up memory if wave loading fails
		if (soundFile != NULL)
			fclose(soundFile);
		if (data)
			delete[] data;
		// return false to indicate the failure to load wave
		return false;
	}
}

void AudioEngine::initialize() {
	alListenerfv(AL_POSITION, listenerPos);
	alListenerfv(AL_VELOCITY, listenerVel);
	alListenerfv(AL_ORIENTATION, listenerOri);

	// Generate buffers, or else no sound will happen!
	alGenSources(NUM_SOURCES, source);
	CheckError();

	// BGM test
	loadWavFile("assets/Cybersong.wav", buffer, &size, &freq, &format);
	CheckError();

	alSourcef(source[0], AL_PITCH, 1.0f);
	alSourcef(source[0], AL_GAIN, 1.0f);
	alSourcefv(source[0], AL_POSITION, source0Pos);
	alSourcefv(source[0], AL_VELOCITY, source0Vel);
	alSourcei(source[0], AL_BUFFER, buffer[0]);
	// alSourcei(source[0], AL_LOOPING, AL_TRUE);
	alSourcePlay(source[0]);

	for (int lx = 0; lx < 1000000000; lx++);
	// SE test
	//loadWavFile("assets/burr.wav", buffer + 1, &size, &freq, &format);
	alSourcef(source[1], AL_PITCH, 1.0f);
	alSourcef(source[1], AL_GAIN, 1.0f);
	alSourcefv(source[1], AL_POSITION, source0Pos);
	alSourcefv(source[1], AL_VELOCITY, source0Vel);
	alSourcei(source[1], AL_BUFFER, buffer[1]);
	// alSourcei(source[1], AL_LOOPING, AL_TRUE);
	alSourcePlay(source[1]);

	return;
}
} // namespace Audio