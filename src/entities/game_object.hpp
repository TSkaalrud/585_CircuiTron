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
	std::string model_path;

  protected:
	Render::Render& render;
	physx::PxTransform& transform;
	std::optional<Render::GroupInstance> model;

  public:
	GameObject(Render::Render& r, std::string mp, physx::PxTransform& pt) : render(r), model_path(mp), transform(pt){};

	void enter() override {
		render.camera_set_fov(50);

		Render::Group group = importModel(model_path, render);
		model.emplace(group);

		Render::GroupInstance track(importModel("assets/The_Coffin_render.glb", render));

		float pi = glm::pi<float>();
		render.create_dir_light({pi, pi, pi}, {1, 1, 1});
		render.create_dir_light({2, 2, 2}, {-1, 0, 1});
		render.create_dir_light({1, 1, 1}, {0, 0, -1});
	}

	void update(float deltaTime) override {
		physx::PxTransform camera(0, 5, -15, physx::PxQuat(physx::PxPi, {0, 1, 0}) * physx::PxQuat(-0.2, {1, 0, 0}));

		render.camera_set_pos(convertTransform(transform.transform(camera)));

		model->setTransform(convertTransform(transform) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
	}
};