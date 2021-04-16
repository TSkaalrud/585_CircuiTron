#pragma once

#include "bike.hpp"
#include "physics/physics.h"
#include "window.hpp"
#include <string>
#include "ui_game.h"

#include <iostream>

class BikePlayer : public Bike {
  private:

  public:
	BikePlayer(
		Window& window, Render::Render& render, int start_place, Render::Group& group, std::vector<glm::vec3> waypoints,
		Audio::AudioEngine& audio, Render::MaterialHandle wallMaterialHandle, UiGame* UI, bool& menuActive)
		: Bike(window, render, start_place, group, audio, wallMaterialHandle, UI, menuActive)  {
		engineAudio->changeGain(0.05);
		FRAGAudio->changeGain(0.3);
		JumpAudio->changeGain(0.15);
		StrafeAudio->changeGain(0.15);
		WADAudio->changeGain(0.25);
		FRAGImpactAudio->changeGain(0.3);
		chassisAudio->changeGain(0.2);
		SlipstreamingAudio->changeGain(1.0);
		this->waypoints = waypoints;
	};

	void update(float deltaTime) override {
		if (getPhysicsActive()) {
			Bike::update(deltaTime);

			physx::PxTransform camera(
				0, 5, -20, physx::PxQuat(physx::PxPi, {0, 1, 0}) * physx::PxQuat(-0.2, {1, 0, 0}));

			if (!getLocked()) {
				checkInput();
				updateWaypoint();
			}
			//std::cout << getSpeed(getId()) << std::endl;
			render.camera_set_pos(convertTransform(getBikeTransform(getId()).transform(camera)));
		}
	}
	
	//functions use glfw keyboard #defines @https : // www.glfw.org/docs/3.3/group__keys.html
	void checkInput() {
		//Driving
		if (!menuActive) {

			if (window.keyPressed(87)) { // w
				if (Slipstreaming) {
					bikeAcceleratePrecise(0, 1.0f);
				} else {
					bikeAcceleratePrecise(0, 0.875f);
				}
			} else {
				if (!window.keyPressed(83)) {
					bikeReleaseGas(0);
				}
			}
			if (window.keyPressed(65)) { // a
				bikeTurnLeft(0);
			} else {
				if (!window.keyPressed(68)) {
					bikeReleaseSteer(0);
				}
			}
			if (window.keyPressed(83)) { // s
				bikeReverse(0);
			} else {
				if (!window.keyPressed(87)) {
					bikeReleaseBrake(0);
				}
			}
			if (window.keyPressed(68)) { // d
				bikeTurnRight(0);
			} else {
				if (!window.keyPressed(65)) {
					bikeReleaseSteer(0);
				}
			}
			if (window.keyPressed(65) && window.keyPressed(58)) {
				bikeReleaseSteer(0);
			}

			// Reset bike
			if (window.keyPressed(82) && resettingCD < 1) { // r
				resetBike();
			}

			// Handbrake
			if (window.keyPressed(340)) { // left shift
				bikeHandbrake(0, 0.15f);
			} else {
				bikeReleaseHandbrake(0);
			}

			// <^> Arrows - bike Booster (jump)
			 if (window.keyPressed(263)) { // left arrow - strafe left
				if (StrafeCD < 1 && getHealth() > 10) {
					modifyHealth(-10);
					bikeBooster(0, 263);
					StrafeCD += 30;
					StrafeAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_BOOST_SFX]);
				}
			}
			 if (window.keyPressed(262)) { // right arrow - strafe right
				if (StrafeCD < 1 && getHealth() > 10) {
					modifyHealth(-10);
					bikeBooster(0, 262);
					StrafeCD += 30;
					StrafeAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_BOOST_SFX]);
				}
			}
			if (window.keyPressed(265) && BoostOn == false) {// up arrow - boost up
				BoostOn = true;
				bikeBooster(0, 265);
				modifyHealth(-10);
				JumpAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_BOOST_SFX]);
			} else if (window.keyPressed(265) && BoostOn == true) {// held booster
				bikeBoosterHold(0, 265);
				modifyHealth(-0.75f);
			} else {
				BoostOn = false;
			}

			// down arrow - WAD
			if (window.keyPressed(264)) { // charge WAD
				WADCharge++;
				modifyHealth(-0.25);
			} else if (WADCharge > 0 && !window.keyPressed(264)) { // release charged WAD
				WADRelease = true;
			}

			// spacebar - Forward Projector Cannon (shoot)
			if (window.keyPressed(32)) {
				if (FRAGCD < 1) {
					FRAGCD += 30;
					modifyHealth(-10);
					fragFire(getId()); // if true, a wall was hit! Plays fire or impact sound
				}
			}

			// left Control - "Control" chassis to right itself
			//if (window.keyPressed(341)) {
			//	bikeControl(0);
			//}
		}
	}

	void updateWaypoint() {
		glm::vec3 target = waypoints[currentWaypoint];
		physx::PxVec3 player = getBikeTransform(0).p;
		float dist = glm::sqrt(glm::pow(target.x - player.x, 2) + glm::pow(target.z - player.z, 2));

		if (dist < 35.0f) {// update to next WP if distance is less than 35
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
		}
	}

};