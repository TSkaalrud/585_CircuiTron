#pragma once

#include "entity.hpp"
#include "render/model_import.hpp"
#include "render/render.hpp"

#include "util.hpp"

#include <foundation/PxTransform.h>
#include <glm/ext/matrix_transform.hpp>
#include <string>

#include <optional>

class GameObject : public Entity {
  private:
  protected:
	Render::Group& group;
	Render::Render& render;
	std::optional<Render::GroupInstance> model;

  public:
	GameObject(Render::Render& r, Render::Group& g)
		: render(r), group(g) {};

	void enter() override {
		model.emplace(group);
	}
};