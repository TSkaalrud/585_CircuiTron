#pragma once

#include "bike.hpp"
#include "window.hpp"
#include "physics/physics.h"

#include <iostream>

class BikePlayer : public Bike {
  private:
	Window& window;

	std::vector<glm::vec3> waypoints;
	int currentWaypoint = 0, nextWaypoint = 1;
  public:
	BikePlayer(Window& window, Render::Render& render, int start_place, Render::Group& group,
			   std::vector<glm::vec3> waypoints)
		: Bike(render, start_place, group), window(window), waypoints(waypoints) {};

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

		if (!getLocked()) {
			checkInput();
		}
		
		updateWaypoint();

		render.camera_set_pos(convertTransform(getBikeTransform(0).transform(camera)));

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

	void updateWaypoint() {
		glm::vec3 target = waypoints[currentWaypoint];
		physx::PxVec3 player = getBikeTransform(0).p;
		float dist = glm::sqrt(glm::pow(target.x - player.x, 2) + glm::pow(target.z - player.z, 2));

		if (dist < 10.0f) {
			currentWaypoint = nextWaypoint;
			if (nextWaypoint == waypoints.size()) {
				currentWaypoint = 0;
				nextWaypoint = 1;
				addLap();
				resetWaypoint();

			} else {
				nextWaypoint++;
				addWaypoint();
			}

			std::cout << "Current Lap: " << getLap() << std::endl;
			std::cout << "Current Waypoint: " << getWaypoint() << std::endl;
		}
	}
};