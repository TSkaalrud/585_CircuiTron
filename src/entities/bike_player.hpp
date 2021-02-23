#pragma once

#include "bike.hpp"
#include "window.hpp"
#include "physics/physics.h"

#include <iostream>

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
		physx::PxTransform camera(0, 5, -20, physx::PxQuat(physx::PxPi, {0, 1, 0}) * physx::PxQuat(-0.2, {1, 0, 0}));

		checkInput();

		render.camera_set_pos(convertTransform(transform.transform(camera)));

		model->setTransform(convertTransform(transform) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
	}

	void checkInput() {
		if (window.keyPressed(87)) {		// w
			//std::cout << 'w pressed' << std::endl;
		} else {
			//std::cout << 'w released' << std::endl;
		}
		
		if (window.keyPressed(65)) {		// a
			//std::cout << 'a pressed' << std::endl;
		} else {
			//std::cout << 'a released' << std::endl;
		}
		
		if (window.keyPressed(83)) {		// s
			//std::cout << 's pressed' << std::endl;
		} else {
			//std::cout << 's released' << std::endl;
		}
		
		if (window.keyPressed(68)) {		// d
			//std::cout << 'd pressed' << std::endl;
		} else {
			//std::cout << 'd released' << std::endl;
		}

		if (window.keyPressed(340)) {		// left shift
			//std::cout << 'left shift pressed' << std::endl;
		} else {
			//std::cout << 'left shift released' << std::endl;
		}
	}
};