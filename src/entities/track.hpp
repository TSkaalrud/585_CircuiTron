#pragma once

#include "game_object.hpp"

class Track : public GameObject {
  private:
  public:
	Track(Render::Render& render, Render::Group& group)
		: GameObject(render, group){};

	void enter() override { 
		model.emplace(group);
		model->setTransform(glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));

		render.camera_set_fov(50);

		float pi = glm::pi<float>();
		render.create_dir_light({pi, pi, pi}, {1, 1, 1});
		render.create_dir_light({2, 2, 2}, {-1, 0, 1});
		render.create_dir_light({1, 1, 1}, {0, 0, -1});
	}

	void update(float deltaTime) override {}
};