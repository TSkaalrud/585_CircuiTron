#pragma once

#include "bike.hpp"
#include "window.hpp"
#include "physics/physics.h"
#include <string>

#include <iostream>

class BikePlayer : public Bike {
  private:
	Window& window;

	//CD = cooldowns for abilities
	int BoostCD = 0;
	int StrafeCD = 0;
	int WADCharge = 0;

	std::vector<glm::vec3> waypoints;
	int currentWaypoint = 0, nextWaypoint = 1;

  public:
	BikePlayer(Window& window, Render::Render& render, int start_place, Render::Group& group, std::vector<glm::vec3> waypoints,
		Audio::AudioEngine& audio)
		: Bike(render, start_place, group, audio), window(window), waypoints(waypoints) 
	{
		engineAudio->loop = true;
	};

	void update(float deltaTime) override {
		physx::PxTransform camera(0, 5, -20, physx::PxQuat(physx::PxPi, {0, 1, 0}) * physx::PxQuat(-0.2, {1, 0, 0}));

		if (!getLocked()) {
			// reduce CD's and regenerate health ***until slipstreaming is in***
			if (BoostCD > 0) {
				BoostCD--;
			}
			if (StrafeCD > 0) {
				StrafeCD--;
			}
			modifyHealth(1);
			checkInput();
			engineAudio->playSound(stereo.buffer[Audio::SOUND_FILE_REV_STEADY_SFX]); // random sound effect example
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
				bikeReleaseBrake(0);
			}
		}
		
		if (window.keyPressed(68)) {		// d
			bikeTurnRight(0);
		} else {
			if (!window.keyPressed(65)) {
				bikeReleaseSteer(0);
			}
		}

		if (window.keyPressed(82)) {		// r
			resetBikePos(0);
		} 

		if (window.keyPressed(340)) {		// left shift
			bikeHandbrake(0);
		} else {
			bikeReleaseHandbrake(0);
		}

		//bike Booster (jump) functions use glfw keyboard #defines @ https://www.glfw.org/docs/3.3/group__keys.html
		if (window.keyPressed(265)) { // up arrow - boost up
			if (BoostCD < 1 && getHealth() > 10) {
				modifyHealth(-10);
				bikeBooster(0, 265);
				BoostCD += 60;
				std::cout << "health: " << getHealth() << std::endl;
			}
		} 

		if (window.keyPressed(263)) { // left arrow - boost left
			if (StrafeCD < 1 && getHealth() > 10) {
				modifyHealth(-10);
				bikeBooster(0, 263);
				StrafeCD += 60;
				std::cout << "health: " << getHealth() << std::endl;
			}
		} 
		if (window.keyPressed(262)) { // right arrow - boost right
			if (StrafeCD < 1 && getHealth() > 10) {
				modifyHealth(-10);
				bikeBooster(0, 262);
				StrafeCD += 60;
				std::cout << "health: " << getHealth() << std::endl;
			}
		} 
		
		// down arrow - WAD
		if (window.keyPressed(264)) { //charge WAD
			WADCharge++;
		} else if (window.keyReleased(264)) { // release WAD
			
			WADCharge = 0;
		}

		// spacebar - Forward Projector Cannon (shoot)
		if (window.keyPressed(32)) {
		}

		// left Control - "Control" chassis to right itself
		if (window.keyPressed(341)) {
			bikeControl(0);
		}

	}

	void updateWaypoint() {
		glm::vec3 target = waypoints[currentWaypoint];
		physx::PxVec3 player = getBikeTransform(0).p;
		float dist = glm::sqrt(glm::pow(target.x - player.x, 2) + glm::pow(target.z - player.z, 2));

		//this radius might need to be tweaked
		if (dist < 25.0f) {
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
			//std::cout << "Current Waypoint: " << getWaypoint() << std::endl;
		}
	}
};