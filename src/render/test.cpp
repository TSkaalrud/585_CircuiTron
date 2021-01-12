#include "render.hpp"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>

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

	auto i = render.create_instance();
	auto mesh = render.create_mesh({.verticies = {{{1, -1, 0}}, {{-1, -1, 0}}, {{0, 1, 0}}}, .indicies = {0, 1, 2}});
	render.instance_set_mesh(i, mesh);
	auto mat = render.create_pbr_material({.albedo = {1, 0, 0, 0}});
	render.instance_set_material(i, mat);

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		render.run();
		glfwSwapBuffers(window);
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}

} // namespace Render
