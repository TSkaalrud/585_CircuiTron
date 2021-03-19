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
	int FRAGCD = 0;
	int WADCharge = 0;
	
	int currentGear = 2;

	std::vector<glm::vec3> waypoints;
	int currentWaypoint = 0, nextWaypoint = 1;

  public:
	BikePlayer(Window& window, Render::Render& render, int start_place, Render::Group& group, std::vector<glm::vec3> waypoints,
		Audio::AudioEngine& audio)
		: Bike(render, start_place, group, audio), window(window), waypoints(waypoints) 
	{
		engineAudio->loop = true;
		gearAudio->loop = false;
		FRAGAudio->loop = false;
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
			if (FRAGCD > 0) {
				FRAGCD--;
			}
			modifyHealth(1);
			checkInput();
			
			//Audio
			//Rev check
			int gear = getBikeGear(0);
			if (currentGear != gear) {
				if (gear == 0) { //reset audio levels when reversing (first gear is used during gear shifts)
					engineAudio->changePitch(1);
					gearAudio->changePitch(1);
					engineAudio->playSound(stereo.buffer[Audio::SOUND_FILE_IDLE_WUB_SFX]);
					gearAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_REV_DOWN2_SFX]);

				} else {

					float delta = 0.1f;
					if (currentGear < gear && gear != 1) {
						// increase pitch (accelerating) (max 2.0)
						delta = delta * (gear - currentGear);

						engineAudio->changePitch(1 + delta);
						gearAudio->changePitch(1 + delta);

						gearAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_REV_UP2_SFX]);
					} else if (currentGear > gear && gear != 1) {
						// decrease pitch (decelerating) (min 0.5)
						delta = delta * (currentGear - gear);
						std::cout << currentGear << " " << gear << std::endl;

						engineAudio->changePitch(1 - delta);
						gearAudio->changePitch(1 - delta);

						gearAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_REV_DOWN2_SFX]);
					}
				}
			}
			// Play pitch corrected revs (on loop)
			engineAudio->playSound(stereo.buffer[Audio::SOUND_FILE_REV_STEADY_SFX]);
			currentGear = gear;
			//std::cout << currentGear << " " << gear << std::endl;


		
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
				JumpAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_BOOST_SFX]);

				std::cout << "health: " << getHealth() << std::endl;

			}
		} 

		if (window.keyPressed(263)) { // left arrow - boost left
			if (StrafeCD < 1 && getHealth() > 10) {
				modifyHealth(-10);
				bikeBooster(0, 263);
				StrafeCD += 60;
				StrafeAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_BOOST_SFX]);

				std::cout << "health: " << getHealth() << std::endl;
			}
		} 
		if (window.keyPressed(262)) { // right arrow - boost right
			if (StrafeCD < 1 && getHealth() > 10) {
				modifyHealth(-10);
				bikeBooster(0, 262);
				StrafeCD += 60;
				StrafeAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_BOOST_SFX]);

				std::cout << "health: " << getHealth() << std::endl;
			}
		} 
		
		// down arrow - WAD
		if (window.keyPressed(264)) { //charge WAD
			WADCharge++;
			WADAudio->loop = true;
			WADAudio->playSound(stereo.buffer[Audio::SOUND_FILE_WAD_SFX]);
		} else if (WADCharge > 0 && window.keyReleased(264)) { // release charged WAD
			physx::PxTransform start = getBikeTransform(getId());
			physx::PxTransform end = getBikeTransform(getId());
			//origin behind the bike's +Z then, with a length based on WADcharge, 
			//extend out +/- x-axes of the bike to create the start and end points
			physx::PxVec3 z = -start.q.getBasisVector2();
			physx::PxVec3 x = start.q.getBasisVector0();
			physx::PxVec3 wallCentre = start.p + 3 * z;
			start.p = wallCentre + 0.5 * x * WADCharge;
			end.p = wallCentre + -0.5 * x * WADCharge;
			makeWallSeg(0, start, end);

			WADAudio->loop = false;
			WADAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_SIZZLE_SFX]);
			WADCharge = 0;
		}

		// spacebar - Forward Projector Cannon (shoot)
		if (window.keyPressed(32)) {
			if (FRAGCD < 1) {
				FRAGCD += 30;
				FRAGAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_GUN_IMPACT2_SFX]);
			} 
			
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