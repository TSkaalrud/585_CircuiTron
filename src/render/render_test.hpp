#pragma once

#include "entities/entity.hpp"
#include "model_import.hpp"
#include "render.hpp"
#include <chrono>
#include <glm/ext/matrix_transform.hpp>
#include <imgui.h>

namespace Render {

class RenderTest : public Entity {
  private:
	Render& render;

  public:
	RenderTest(Render& render) : render(render){};
	void enter() override {
		render.camera_set_fov(50);

		import_scene("assets/Bike_Final.glb", render);

		float pi = glm::pi<float>();
		render.create_dir_light({pi, pi, pi}, {1, 1, 1});
		render.create_dir_light({2, 2, 2}, {-1, 0, 1});
		render.create_dir_light({1, 1, 1}, {0, 0, -1});
	}

	void update(float deltaTime) override {
		static auto start_time = std::chrono::high_resolution_clock::now();
		auto current_time = std::chrono::high_resolution_clock::now();
		auto seconds = std::chrono::duration_cast<std::chrono::duration<float>>(current_time - start_time).count();
		const float dist = 6;
		const float speed = 1;

		vec3 camera = {sin(seconds * speed) * dist, 0, cos(seconds * speed) * dist};
		mat4 cameraPos = lookAt(camera, vec3{0, 0, 0}, vec3{0, 1, 0});

		render.camera_set_pos(cameraPos);
	}
};
} // namespace Render