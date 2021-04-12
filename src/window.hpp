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

	void close() { glfwSetWindowShouldClose(window, true); }

	void beginFrame();
	void endFrame();

	bool keyPressed(int key) { return glfwGetKey(window, key) == GLFW_PRESS; }

	bool mouseButton(int button) { return glfwGetMouseButton(window, button) == GLFW_PRESS; }
	struct Cursor {
	  private:
		friend Window;
		void apply(double xpos, double ypos) {
			this->deltax = xpos - this->xpos;
			this->deltay = ypos - this->ypos;
			this->xpos = xpos;
			this->ypos = ypos;
		}
		void add(double xpos, double ypos) {
			this->deltax += xpos;
			this->deltay += ypos;
			this->xpos += xpos;
			this->ypos += ypos;
		}
		void reset() {
			this->deltax = 0;
			this->deltay = 0;
		}

	  public:
		double xpos, ypos, deltax, deltay;
	};
	Cursor cursor;
	Cursor scroll;

	enum CursorMode { Normal = GLFW_CURSOR_NORMAL, Hidden = GLFW_CURSOR_HIDDEN, Disabled = GLFW_CURSOR_DISABLED };
	void setCursorMode(CursorMode mode) { glfwSetInputMode(window, GLFW_CURSOR, mode); }
};