#pragma once

#include "entity.hpp"
#include "render/render.hpp"
#include "render/model_import.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <string>

class GameObject : public Entity {
	private:
		Render::Render& render;
		std::string model_path;

	protected:
		glm::mat4 position;

	public:
		GameObject(Render::Render& r, std::string mp) : render(r){
			model_path = mp;
		};

		void enter() override {
			render.camera_set_fov(50);

			Render::Group group = importModel(model_path, render);

			Render::GroupInstance model(group);

			float pi = glm::pi<float>();
			render.create_dir_light({pi, pi, pi}, {1, 1, 1});
			render.create_dir_light({2, 2, 2}, {-1, 0, 1});
			render.create_dir_light({1, 1, 1}, {0, 0, -1});
		}

		void update(float deltaTime) override {
			const float dist = 250;
			const float speed = 1;

			glm::vec3 camera = {sin(deltaTime * speed) * dist, 0, cos(deltaTime * speed) * dist};
			glm::mat4 cameraPos = lookAt(camera, glm::vec3{0, 0, 0}, glm::vec3{0, 1, 0});

			render.camera_set_pos(cameraPos);
		}
};