//Audio
#include "Audio/audioEngine.h"
#include "Audio/audioInstance.h"
#include <AL/al.h>
#include <AL/alc.h>
//#include "../../out/build/x64-Debug (default)/_deps/openal-src/include/AL/al.h"
//#include "../../out/build/x64-Debug (default)/_deps/openal-src/include/AL/alc.h"


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

	// Create entitity manager
	EntityManager e_manager;

	// Set up variables for fixed and variable timestep
	float timestep = 1.f / 60.f;
	float time = 0;
	auto past = std::chrono::high_resolution_clock::now();

	// initialize physics
	initPhysics();


	if (args.size() > 1) {
		e_manager.addEntity(std::make_unique<ModelView>(render, args.at(1)));
		e_manager.addEntity(std::make_unique<OrbitCam>(render, window));
	} else {
		e_manager.addEntity(std::make_unique<Game>(window, render, 2, e_manager));
	}

	//initialize audio
	Audio::AudioEngine stereo = Audio::AudioEngine();
	stereo.initialize();
	AudioInstance* bgm = new AudioInstance();
	bgm->playSound(stereo.buffer[Audio::SOUND_FILE_CYBERSONG_BGM]); //Song
	AudioInstance* sfx_1 = new AudioInstance();
	sfx_1->playSound(stereo.buffer[Audio::SOUND_FILE_AMBIENCE_BGM]); //ambient environment sounds
	AudioInstance* sfx_2 = new AudioInstance();
	sfx_2->playSound(stereo.buffer[Audio::SOUND_FILE_GUN_IMPACT_SFX]); //random sound effect example


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


		

/*
		// Probably want to combine all this into a render()
		render.run();
		// To actually draw something with the render:
		// 1. Load models
		// 2. Create materials
		// 3. Create instances
		// 4. Set camera position and fov
		// 5. (Optional) Add lights
		// Future API to simplify some of these steps?
		// Probably done by various entities once that system is functional?

		e_manager.addEntitiesAfterFrame(); // not sure if this belong after the ImGUI stuff

		// Draw the framerate counter
		// This is replaced by Profiler if I get it working better
		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin(
			"Profiler", nullptr,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text("%.1f fps", ImGui::GetIO().Framerate);
		ImGui::End();

		// All imgui commands must happen before here
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// Swap the render buffers and wait for the next frame
		// Should probably be the last thing in the game loop
		glfwSwapBuffers(window);
*/
		window.endFrame();

	}

	return 0;
}