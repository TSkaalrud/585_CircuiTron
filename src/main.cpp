// Audio
#include "Audio/audioEngine.h"
#include "Audio/audioInstance.h"
#include "entities/entity_manager.hpp"
#include "entities/game.hpp"
#include "entities/model_view.hpp"
#include "entities/orbit_cam.hpp"
#include "physics/physics.h"
#include "window.hpp"
#include <chrono>
#include <iostream>

int main(int argc, char* argv[]) {
	// Read in the command line args
	std::vector<std::string> args;
	args.assign(argv, argv + argc);

	Window window;

	Render::Render& render = window.getRender();
	// render.set_skybox_rect_texture(importSkybox("assets/skyboxes/5TH_AVENUE.hdr", render));
	// render.set_skybox_rect_texture(importSkybox("assets/skyboxes/BROADWAY_LAFAYETTE_STATION_2.hdr", render));
	// render.set_skybox_rect_texture(importSkybox("assets/neurathen_rock_castle_4k.hdr", render));
	render.set_skybox_rect_texture(importSkybox("assets/skyboxes/SPACE-1.hdr", render));

	// Create entitity manager
	EntityManager e_manager;

	// Set up variables for fixed and variable timestep
	float timestep = 1.f / 60.f;
	float time = 0;
	auto past = std::chrono::high_resolution_clock::now();

	// initialize physics
	initPhysics();

		// initialize audio
	Audio::AudioEngine stereo = Audio::AudioEngine();
	stereo.initialize();
	AudioInstance* bgm = new AudioInstance();
	bgm->gain = 0.0;
	bgm->playSound(stereo.buffer[Audio::SOUND_FILE_CYBERSONG_BGM]); // Song
	AudioInstance* ambiance = new AudioInstance();
	ambiance->gain = 0.0;
	ambiance->playSound(stereo.buffer[Audio::SOUND_FILE_AMBIENCE_BGM]); // ambient environment sounds

	if (args.size() > 1) {
		e_manager.addEntity(std::make_unique<ModelView>(render, args.at(1)));
		e_manager.addEntity(std::make_unique<OrbitCam>(render, window));
	} else {
		e_manager.addEntity(std::make_unique<Game>(window, render, 1, e_manager, stereo));
	}

	// Loop will continue until "X" on window is clicked.
	// We may want more complex closing behaviour
	while (!window.shouldClose()) {
		window.beginFrame();

		e_manager.addEntitiesAfterFrame();

		// 1 for variable, 0 for fixed
		if (1) {
			auto now = std::chrono::high_resolution_clock::now();
			timestep = std::chrono::duration_cast<std::chrono::duration<float>>(now - past).count();
			past = now;
		}
		// simulate();
		stepPhysics();

		time += timestep;

		// Calls the update function all of the entities added to the manager
		// Maybe not needed for now
		e_manager.update(time);

		// sound();

		window.endFrame();
	}
	return 0;
}