#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <AL/al.h>
#include <AL/alc.h>
//#include "../../out/build/x64-Debug (default)/_deps/openal-src/include/AL/al.h"
//#include "../../out/build/x64-Debug (default)/_deps/openal-src/include/AL/alc.h"


class AudioInstance {

	public:
		//buffer takes in the sound
	  AudioInstance() {
		  alGenSources(1, &source);
		  alSourcef(source, AL_PITCH, pitch);
		  alSourcef(source, AL_GAIN, gain);
		  alSource3f(source, AL_POSITION, pos[0], pos[1], pos[2]);
		  alSource3f(source, AL_VELOCITY, vel[0], vel[1], vel[2]);
		  alSourcei(source, AL_LOOPING, loop);
		  alSourcei(source, AL_BUFFER, buffer);

	  };
	  ~AudioInstance(){ 
		  alDeleteSources(1, &source);
	  };
	  void initialize() { ;
	  };
	  void playSound(const ALuint& sound_ID){ 
		  if (sound_ID != buffer) {
			  buffer = sound_ID;
			  alSourcei(source, AL_BUFFER, buffer);
		  }
		  alSourcePlay(source);
	  };
	  void stopSound(){
	  };
	  bool isSoundPlaying(){
	  };
	  void cleanup(){
	  };
	
	//source holds the source audio
	ALuint source;
	//holds the state of the source audio
	ALint source_state;

	float pitch = 1.f;
	float gain = 1.f;
	float pos[3] = {0, 0, 0};
	float vel[3] = {0, 0, 0};
	bool loop = false;
	ALuint buffer = 0;
};