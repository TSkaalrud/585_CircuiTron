#pragma once

#include "bike.hpp"
#include "window.hpp"
#include "physics/physics.h"

class BikePlayer : public Bike {
  private:
	Window& window;
  public:
	BikePlayer(Window& window, Render::Render& render, int start_place, physx::PxTransform& pt, Render::Group& group)
		: Bike(render, start_place, pt, group), window(window){};

	void enter() override {
		render.camera_set_fov(50);

		model.emplace(group);

		float pi = glm::pi<float>();
		render.create_dir_light({pi, pi, pi}, {1, 1, 1});
		render.create_dir_light({2, 2, 2}, {-1, 0, 1});
		render.create_dir_light({1, 1, 1}, {0, 0, -1});
	}

	void update(float deltaTime) override {
		physx::PxTransform camera(0, 100, -100, physx::PxQuat(physx::PxPi, {0, 1, 0}) * physx::PxQuat(-1, {1, 0, 0}));

		checkInput();

		render.camera_set_pos(convertTransform(transform.transform(camera)));

		model->setTransform(convertTransform(getBikeTransform(0)) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
	}

	void checkInput() {
		if (window.keyPressed(87)) {		// w
			bikeAccelerate(0);
		} else {
			if (!window.keyPressed(83)) {
				bikeReleaseGas(0);
			}
		}
		
		if (window.keyPressed(65)) {		// a
			bikeTurnLeft(0);
		} else {
			if (!window.keyPressed(68)) {
				bikeReleaseSteer(0);
			}
		}
		
		if (window.keyPressed(83)) {		// s
			bikeReverse(0);
		} else {
			if (!window.keyPressed(87)) {
				bikeReleaseGas(0);
			}
		}
		
		if (window.keyPressed(68)) {		// d
			bikeTurnRight(0);
		} else {
			if (!window.keyPressed(65)) {
				bikeReleaseSteer(0);
			}
		}

		if (window.keyPressed(340)) {		// left shift
			bikeBreak(0);
		} else {
			bikeReleaseBrake(0);
		}
	}
};