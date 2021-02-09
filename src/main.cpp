#include "entities/entity.hpp"
#include "entities/entity_manager.hpp"
#include "physics/physics.h"
#include "render/render.hpp"
#include "render/render_test.hpp"
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>

int main(int argc, char* argv[]) {
	// Read in the command line args
	std::vector<std::string> args;
	args.assign(argv, argv + argc);

	// GLFW Init
	// Print GLFW errors
	glfwSetErrorCallback([](int, const char* description) { fprintf(stderr, "Error: %s\n", description); });

	if (!glfwInit())
		exit(EXIT_FAILURE);

	// I need OpenGL 4.5 Core
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// MSAA x8 (If I implement post-processing, I'll do AA there, so this could be removed in the future)
	glfwWindowHint(GLFW_SAMPLES, 64);

	// Debug Contexts are helpful but slow; Only enable in debug builds
#ifndef NDEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

	GLFWwindow* window = glfwCreateWindow(1280, 720, "CircuiTron", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	// Uncomment to disable v-sync
	// glfwSwapInterval(0);

	Render::Render render(glfwGetProcAddress);
	{
		// Render needs to be resized before it can be used
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		render.resize(width, height);
	}

	// Resize render on window resize
	// My callback uses the userpoint to store render
	// This may need to be reworked as we may need the user pointer for other things as well.
	glfwSetWindowUserPointer(window, &render);
	auto resizeCallback = [](GLFWwindow* window, int width, int height) {
		Render::Render* render = static_cast<Render::Render*>(glfwGetWindowUserPointer(window));
		render->resize(width, height);
	};
	glfwSetFramebufferSizeCallback(window, resizeCallback);

	// Init imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(NULL);

	// Create entitity manager
	EntityManager e_manager;

	// Setting up fixed timestep, change to variable later!
	float timestep = 1.f / 60.f;
	float time = 0;

	// if (args.size() > 1 && args.at(1) == "view") {
	e_manager.addEntity(std::make_unique<Render::RenderTest>(render));
	// }

	// initialize physics
	initPhysics(true);

	// Loop will continue until "X" on window is clicked.
	// We may want more complex closing behaviour
	while (!glfwWindowShouldClose(window)) {

		// All imgui commands must happen after here
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		// This is probably part of input()
		glfwPollEvents();
		// input();

		// simulate();
		stepPhysics(true);

		// Fixed timestep for now
		time += timestep;	

		// Calls the update function all of the entities added to the manager
		// Maybe not needed for now
		e_manager.update(time);

		// sound();

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

		e_manager.addEntitiesAfterFrame();		//not sure if this belong after the ImGUI stuff

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
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}