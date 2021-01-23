#include "render/render.hpp"
#include <GLFW/glfw3.h>
#include <iostream>

int main() {

	//this throws an error because there is no window
	Render::Render render(glfwGetProcAddress);

	while (1) {
		//input();
		//simulate();
		render.run();
		//sound();
	}

	//Render::render_test();
	return 0;
}