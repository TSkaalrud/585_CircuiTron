#pragma once

#include "entity.hpp"
#include "render/render.hpp"
#include <chrono>

class FixedOrbitCam : public Entity {
  private:
	Render::Render& render;

  public:
	FixedOrbitCam(Render::Render& render) : render(render){};
	void enter() override { render.camera_set_fov(50); }

	void update(float deltaTime) override {
		static auto start_time = std::chrono::high_resolution_clock::now();
		auto current_time = std::chrono::high_resolution_clock::now();
		auto seconds = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - start_time).count();
		const float dist = 3;
		const float speed = 1;

		glm::vec3 camera = {sin(seconds * speed) * dist, 0, cos(seconds * speed) * dist};
		glm::mat4 cameraPos = glm::inverse(glm::lookAt(camera, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0}));

		render.camera_set_pos(cameraPos);
	}
};