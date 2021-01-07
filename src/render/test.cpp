// #include "gl.hpp"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>

static void error_callback(int error, const char* description) { fprintf(stderr, "Error: %s\n", description); }

void render_test() {

	glfwSetErrorCallback(error_callback);

	if (!glfwInit())
		exit(EXIT_FAILURE);

	GLFWwindow* window = glfwCreateWindow(1280, 720, "CircuiTron", NULL, NULL);
	if (!window) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}
	// if (loadGL(glfwGetProcAddress)) {
	// 	glfwDestroyWindow(window);
	// 	glfwTerminate();
	// 	exit(EXIT_FAILURE);
	// }
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
}
