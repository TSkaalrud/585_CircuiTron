#include "model_import.hpp"
#include "profiler/profiler.hpp"
#include "render.hpp"
#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

namespace Render {

static void error_callback(int, const char* description) { fprintf(stderr, "Error: %s\n", description); }

void render_test() {
	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_SAMPLES, 8);
#ifndef NDEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
#endif

	GLFWwindow* window = glfwCreateWindow(1280, 720, "CircuiTron", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	glfwMakeContextCurrent(window);
	// glfwSwapInterval(0);

	Render render(glfwGetProcAddress);
	glfwSetWindowUserPointer(window, &render);
	{
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);
		render.resize(width, height);
	}
	auto resizeCallback = [](GLFWwindow* window, int width, int height) {
		Render* render = static_cast<Render*>(glfwGetWindowUserPointer(window));
		render->resize(width, height);
	};
	glfwSetFramebufferSizeCallback(window, resizeCallback);

	render.camera_set_fov(50);

	import_scene("assets/DamagedHelmet.glb", render);

	float pi = glm::pi<float>();
	render.create_dir_light({pi, pi, pi}, {1, 1, 1});
	render.create_dir_light({2, 2, 2}, {-1, 0, 1});
	render.create_dir_light({1, 1, 1}, {0, 0, -1});

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(NULL);

	Profiler profiler;

	while (!glfwWindowShouldClose(window)) {
		profiler.newFrame("Setup");
		glfwPollEvents();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		{
			static auto start_time = std::chrono::high_resolution_clock::now();
			auto current_time = std::chrono::high_resolution_clock::now();
			auto seconds = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - start_time).count();
			const float dist = 3;
			const float speed = 1;

			vec3 camera = {sin(seconds * speed) * dist, 0, cos(seconds * speed) * dist};
			mat4 cameraPos = lookAt(camera, vec3{0, 0, 0}, vec3{0, 1, 0});

			render.camera_set_pos(cameraPos);
		}

		profiler.section("Rendering");
		render.run();

		profiler.section("imgui");
		profiler.draw();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		profiler.section("Present");
		glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}

} // namespace Render
