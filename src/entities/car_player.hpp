#pragma once

#include "car.hpp"

class CarPlayer : public Car {
  private:
  public:
	CarPlayer(Render::Render& render, int start_place, physx::PxTransform& pt, Render::Group& group)
		: Car(render, start_place, pt, group){};

	void enter() override {
		render.camera_set_fov(50);

		model.emplace(group);

		float pi = glm::pi<float>();
		render.create_dir_light({pi, pi, pi}, {1, 1, 1});
		render.create_dir_light({2, 2, 2}, {-1, 0, 1});
		render.create_dir_light({1, 1, 1}, {0, 0, -1});
	}

	void update(float deltaTime) override {
		physx::PxTransform camera(0, 5, -20, physx::PxQuat(physx::PxPi, {0, 1, 0}) * physx::PxQuat(-0.2, {1, 0, 0}));

		render.camera_set_pos(convertTransform(transform.transform(camera)));

		model->setTransform(convertTransform(transform));
	}
};