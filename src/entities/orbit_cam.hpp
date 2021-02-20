#pragma once

#include "entity.hpp"
#include "render/render.hpp"
#include "window.hpp"
#include <imgui.h>

class OrbitCam : public Entity {
  private:
	Render::Render& render;
	Window& window;

	const float speed = -1.0f / 200.0f;
	float xAngle, yAngle;
	const float pi = glm::pi<float>();

  public:
	OrbitCam(Render::Render& render, Window& window) : render(render), window(window){};
	void enter() override { render.camera_set_fov(50); }

	void update(float) override {
		if (window.mouseButton(GLFW_MOUSE_BUTTON_LEFT)) {
			window.setCursorMode(Window::CursorMode::Disabled);
			xAngle += window.cursor.deltax * speed;
			yAngle += window.cursor.deltay * speed;
			yAngle = glm::clamp<float>(yAngle, -pi / 2, pi / 2);

		} else {
			window.setCursorMode(Window::CursorMode::Normal);
		}
		const float dist = 3;

		ImGui::Begin(
			"Cursor", nullptr,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_AlwaysAutoResize |
				ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoSavedSettings);
		ImGui::Text("(%.1f,%.1f)", xAngle, yAngle);
		ImGui::End();

		glm::mat4 cameraPos = glm::rotate(glm::mat4(1.0f), xAngle, {0, 1, 0}) *
			glm::rotate(glm::mat4(1.0f), yAngle, {1, 0, 0}) * glm::translate(glm::mat4(1.0f), glm::vec3({0, 0, dist}));

		render.camera_set_pos(cameraPos);
	}
};