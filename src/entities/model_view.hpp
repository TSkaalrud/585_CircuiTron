#pragma once

#include "entities/entity.hpp"
#include "render/model_import.hpp"
#include "render/render.hpp"
#include <string>

class ModelView : public Entity {
  private:
	Render::Render& render;
	Render::Group group;

  public:
	ModelView(Render::Render& render, std::string modelPath) : render(render), group(importModel(modelPath, render)){};
	void enter() override {

		Render::GroupInstance model(group);

		float pi = glm::pi<float>();
		render.create_dir_light({pi, pi, pi}, {1, 1, 1});
		render.create_dir_light({2, 2, 2}, {-1, 0, 1});
		render.create_dir_light({1, 1, 1}, {0, 0, -1});
	}

	void update(float) override {}
};