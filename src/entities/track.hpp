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
	}

	void update(float deltaTime) override {}
};