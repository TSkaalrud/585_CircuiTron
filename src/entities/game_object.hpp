#pragma once

#include "entity.hpp"
#include "render/render.hpp"
#include "render/model_import.hpp"

#include "util.hpp"

#include <foundation/PxTransform.h>
#include <glm/ext/matrix_transform.hpp>
#include <string>

#include <optional>

class GameObject : public Entity {
	private:
		Render::Render& render;
		std::string model_path;
		physx::PxTransform transform;
		std::optional<Render::GroupInstance> model;

	public:
		GameObject(Render::Render& r, std::string mp, physx::PxTransform& pt) : render(r) {
			model_path = mp;
			transform = pt;
		};

		void enter() override {
			render.camera_set_fov(50);

			Render::Group group = importModel(model_path, render);

			model.emplace(group);

			float pi = glm::pi<float>();
			render.create_dir_light({pi, pi, pi}, {1, 1, 1});
			render.create_dir_light({2, 2, 2}, {-1, 0, 1});
			render.create_dir_light({1, 1, 1}, {0, 0, -1});
		}

		void update(float deltaTime) override {
			const float dist = 10;
			const float speed = 1;

			glm::vec3 camera = {10, 5, 0};
			glm::mat4 cameraPos = lookAt(camera, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0});

			render.camera_set_pos(cameraPos);

			model -> setTransform(convertTransform(transform));
		}
};