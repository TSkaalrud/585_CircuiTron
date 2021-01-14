#include "model_import.hpp"
#include "render.hpp"
#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>

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

	render.camera_set_fov(70);

	import_scene("assets/horse.obj", render);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		static auto start_time = std::chrono::high_resolution_clock::now();
		auto current_time = std::chrono::high_resolution_clock::now();
		auto seconds = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - start_time).count();
		const float dist = 2;

		vec3 camera = {sin(seconds) * dist, 1, cos(seconds) * dist};
		mat4 cameraPos = lookAt(camera, vec3{0, 0.5, 0}, vec3{0, 1, 0});

		render.camera_set_pos(cameraPos);

		render.run();
		glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}

} // namespace Render
