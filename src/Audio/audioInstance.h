#pragma once
#define _CRT_SECURE_NO_WARNINGS
//#include <AL/al.h>
//#include <AL/alc.h>
#include "../../out/build/x64-Debug (default)/_deps/openal-src/include/AL/al.h"
//#include "../../out/build/x64-Debug (default)/_deps/openal-src/include/AL/alc.h"

class AudioInstance {

	public:
		//buffer takes in the sound
	AudioInstance(ALuint* buffer);
	//~AudioInstance();
	void initialize();
	void playSound();
	void stopSound();
	bool isSoundPlaying();
	void cleanup();
	
	//source holds the source audio
	ALuint source;
	//holds the state of the source audio
	ALint source_state;

};