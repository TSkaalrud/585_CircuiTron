#include "Audio/audioEngine.h"
#include "Audio/audioInstance.h"
#include "entities/entity_manager.hpp"
#include "entities/game.hpp"
#include "entities/model_view.hpp"
#include "entities/orbit_cam.hpp"
#include "entities/ui_test.hpp"
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


	bool menuActive = true;

	if (args.size() > 1) {//render test
		e_manager.addEntity(std::make_unique<ModelView>(render, args.at(1)));
		e_manager.addEntity(std::make_unique<OrbitCam>(render, window));
		e_manager.addEntity(std::make_unique<UiTest>(render, window));
	} else {//game
		//initialize UI
		std::unique_ptr<UiGame> UI = std::make_unique<UiGame>(render, window, menuActive);
		UiGame* game_UI = UI.get();
		e_manager.addEntity(std::move(UI));
		//initialize game
		e_manager.addEntity(std::make_unique<Game>(window, render, 4, e_manager, stereo, game_UI, menuActive));
	}

	// Loop will continue until "X" on window is clicked.
	// We may want more complex closing behaviour
	while (!window.shouldClose()) {
		window.beginFrame();

		e_manager.addEntitiesAfterFrame();

		// 1 for variable, 0 for fixed
		if (0) {
			auto now = std::chrono::high_resolution_clock::now();
			timestep = std::chrono::duration_cast<std::chrono::duration<float>>(now - past).count();
			past = now;
		}
		if (!menuActive) {// pause physics while in menu
			// simulate;
			stepPhysics(timestep);
		}
		time += timestep;

		// Calls the update function all of the entities added to the manager
		// Maybe not needed for now
		e_manager.update(timestep);

		// sound();

		window.endFrame();
	}
	return 0;
}