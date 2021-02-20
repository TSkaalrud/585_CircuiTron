#pragma once

#include "render/render.hpp"
#include <GLFW/glfw3.h>

class Window {
  private:
	Render::Render render;
	GLFWwindow* window;

	Window(GLFWwindow*);

  public:
	Window();
	Window(const Window&) = delete;
	~Window() {
		glfwDestroyWindow(window);
		glfwTerminate();
	}

	Render::Render& getRender() { return render; }

	bool shouldClose() { return glfwWindowShouldClose(window); }

	void beginFrame();
	void endFrame();

	bool keyPressed(int key) { return glfwGetKey(window, key) == GLFW_PRESS; }
	bool mouseButton(int button) { return glfwGetMouseButton(window, button) == GLFW_PRESS; }
	struct Cursor {
		double xpos, ypos, deltax, deltay;
	};
	Cursor cursor;

	enum CursorMode { Normal = GLFW_CURSOR_NORMAL, Hidden = GLFW_CURSOR_HIDDEN, Disabled = GLFW_CURSOR_DISABLED };
	void setCursorMode(CursorMode mode) { glfwSetInputMode(window, GLFW_CURSOR, mode); }
};