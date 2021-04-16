#pragma once

#include "bike.hpp"
#include "window.hpp"
#include "ui_game.h"


#include <iostream>
#include <vector>

class BikeAI : public Bike {
  private:
	std::vector<std::vector<glm::vec3>> ai_waypoints; // a list of waypoints for each ai bike
	// direction of the bike's current target
	bool left = false;
	bool right = false;
	int buffer = 60; // timer before AI bikes will start driving
	std::vector<int>& waypointOptions;
	//float slowVery = 0.8;
	float slow = 0.900;
	float slowSlip = 0.950;
	float fast = 0.975;
	float fastSlip = 1.f;
	float handbrakingStrength = 0.2f;
	bool cornering = false;
	bool corneringHard = false;
	int lane;

  public:
	BikeAI(
		Window& window, Render::Render& render, int start_place, Render::Group& group,
		std::vector<std::vector<glm::vec3>> ai_waypoints, Audio::AudioEngine& audio,
		Render::MaterialHandle wallMaterialHandle, UiGame* UI, bool& menuActive, std::vector<int>& waypointOptions)
		: Bike(window, render, start_place, group, audio, wallMaterialHandle, UI, menuActive),
		  ai_waypoints(ai_waypoints), waypointOptions(waypointOptions) {
		engineAudio->changeGain(0.025);
		gearAudio->changeGain(0.025);
		FRAGAudio->changeGain(0.10);
		JumpAudio->changeGain(0.075);
		StrafeAudio->changeGain(0.075);
		WADAudio->changeGain(0.1);
		FRAGImpactAudio->changeGain(0.10);
		chassisAudio->changeGain(0.1);
		SlipstreamingAudio->changeGain(0.5);
		
		srand((unsigned int)time(NULL));
		//getNewLane(waypointOptions);
		if (waypointOptions.size() > 0) {
			int laneIndex = rand() % (waypointOptions.size()-5);
			lane = waypointOptions[laneIndex];
			waypoints = ai_waypoints[lane];
			waypointOptions.erase(waypointOptions.begin() + laneIndex);
		}
		//waypoints = ai_waypoints[1];

	};

	void update(float deltaTime) override {
		if (getPhysicsActive()) {
			Bike::update(deltaTime);
			if (!getLocked()) {
				if (buffer < 0) {
					//isCornering();
					followWaypoint();
					useAbilities();
				} else {
					buffer--;
				}
			}
		}
	}

	void followWaypoint() {
		glm::vec3 target = waypoints[currentWaypoint];
		physx::PxTransform ai = getBikeTransform(getId());

		physx::PxVec3 heading = ai.q.getBasisVector2() + ai.p;

		physx::PxVec3 ai_pos = ai.p;

		float d = (target.x - ai_pos.x) * (heading.z - ai_pos.z) - (target.z - ai_pos.z) * (heading.x - ai_pos.x);
		float dist = glm::sqrt(glm::pow(target.x - ai_pos.x, 2) + glm::pow(target.z - ai_pos.z, 2));

		if (dist < 25) {// update to next WP if distance is less than 25
			currentWaypoint = nextWaypoint;
			if (nextWaypoint == waypoints.size()) {
				currentWaypoint = 0;
				nextWaypoint = 1;
				//std::cout << getLap() << std::endl;
				//waypoints = ai_waypoints[5+getLap()];
				//waypoints = ai_waypoints[0];
				addLap();
				resetWaypoint();
				getNewLane(waypointOptions);
			} else {
				nextWaypoint++;
				addWaypoint();
			}
		}

		physx::PxTransform player = getBikeTransform(0);
		physx::PxVec3 player_pos = player.p;

		glm::vec3 toTarget(target.x - ai_pos.x, target.y - ai_pos.y, target.z - ai_pos.z);
		glm::vec3 toHeading(heading.x - ai_pos.x, heading.y - ai_pos.y, heading.z - ai_pos.z);

		float dot = glm::dot(toTarget, toHeading);
		float mag = glm::length(toTarget) * glm::length(toHeading);

		float angle = glm::acos(dot / mag);

		float angleRange = (3.14f - 0.0f);
		float radiusRange = (1.0f - 0.0f);

		float radius = (((angle - 0.0f) * radiusRange) / angleRange) - 0.0f;
		//std::cout << radius << std::endl;
		isCorneringAlt(radius);

		//if angle to waypoint too high booster
		if (BoostCD <= 0) {
			if (angle > 0.45) {
				if (d > 0) { // left
					bikeBooster(getId(), 263);
					BoostCD += 60;
					StrafeAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_BOOST_SFX]);
				} else if (d < 0) { // right
					bikeBooster(getId(), 262);
					BoostCD += 60;
					StrafeAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_BOOST_SFX]);
				}
			}
		}
		if (dist > 25.0f) {
			if (d > 0) { //left
				bikeReleaseSteer(getId());
				bikeTurnPrecise(getId(), glm::min(3.5f * radius, 1.f));
				if (Slipstreaming) {
					if (cornering) {
						bikeAcceleratePrecise(getId(), slowSlip);
						if (corneringHard && (getSpeed(getId()) > 25.f)) {
							bikeHandbrake(getId(), handbrakingStrength);
						} else {
							bikeReleaseHandbrake(getId());
						}
					} else {
						bikeAcceleratePrecise(getId(), fastSlip);
						bikeReleaseHandbrake(getId());

					}
				} else {
					if (cornering) { 
						bikeAcceleratePrecise(getId(), slow);
						if (corneringHard && (getSpeed(getId()) > 25.f)) {
							bikeHandbrake(getId(), handbrakingStrength);
						} else {
							bikeReleaseHandbrake(getId());
						}
					} else {
						bikeAcceleratePrecise(getId(), fast);
						bikeReleaseHandbrake(getId());

					}
				}
			} else if (d < 0) { //right
				bikeReleaseSteer(getId());
				bikeTurnPrecise(getId(), glm::max(-3.5f * radius, -1.f));
				if (Slipstreaming) {
					bikeAcceleratePrecise(getId(), 0.9);
				} else {
					bikeAcceleratePrecise(getId(), 0.825f);
				}
			} else { // on
				bikeReleaseSteer(getId());
				if (Slipstreaming) {
					bikeAcceleratePrecise(getId(), 0.9);
				} else {
					bikeAcceleratePrecise(getId(), 0.825f);
				}
			}
		} else {
			//bikeReleaseAll(getId());
			
			//std::cout << "unneccessarily slow" << std::endl;
		}
	}

	void getNewLane(std::vector<int>& waypointOptions) {
		if (waypointOptions.size() > 0) {
			int laneIndex = rand() % waypointOptions.size();
			lane = waypointOptions[laneIndex];
			waypoints = ai_waypoints[lane];
			waypointOptions.erase(waypointOptions.begin() + laneIndex);
		}
	}

	// Hitscan bike cannon, the FRAG but for AI's
	bool AIfragFire(int bike, int range) {
		auto wallPointer = fragRay(bike, range);

		if (wallPointer != NULL) {
			// mark walls for deletion here
			markWallBroken(
				static_cast<wallUserData*>(wallPointer)->bikeNumber,
				static_cast<wallUserData*>(wallPointer)->wallIndex);

			FRAGImpactAudio->playSoundOverride(stereo.buffer[Audio::SOUND_FILE_GUN_IMPACT2_SFX]);
			modifyHealth(-6);
			FRAGCD += 150;

			return true;
		} else {
			// AI doesn't fire if it won't hit
			return false;
		}
	}

	void AIBooster() {
		auto wallPointer = fragRay(getId(), 15);
		if (wallPointer != NULL) {
			AIbikeBooster(getId(), 265);
			BoostCD += 150;
		}
	}

	void useAbilities() { 
		if (FRAGCD <= 0 ) {
			if (health > 75) {
				AIfragFire(getId(), 50);
			} else if (health > 25 || (health > 5 && Slipstreaming) ) {
				AIfragFire(getId(), 5);
			}
		} 
		if (BoostCD <= 0 && !cornering && health > 25) {
			AIBooster(); 
		} 
		if (health > 95 && WADCharge < 120) {
			WADCharge++;
			modifyHealth(-0.25);
		} else if (WADCharge > 0 && WADRelease == false) {
			WADRelease = true;
		}
	}

	// experimental estimates of where the curves are to slow down for (on The Coffin map)
	void isCornering() {//~20-90 big curve 140-185 little curve? 20-76 and 130-180?
		cornering = (currentWaypoint > 30 && currentWaypoint < 90) || (currentWaypoint > 135 && currentWaypoint < 195);
	}

	void isCorneringAlt(float radius) { 
		cornering = (radius > 0.040);
		corneringHard = (radius > 0.1);
		//if (corneringHard) {
		//	std::cout << radius << std::endl;
		//}

	}
};