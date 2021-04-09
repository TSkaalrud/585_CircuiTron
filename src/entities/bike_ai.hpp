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
	float slow = 0.85;
	float slowSlip = 0.9;
	float fast = 0.95;
	float fastSlip = 1.f;

  public:
	BikeAI(
		Window& window, Render::Render& render, int start_place, Render::Group& group,
		std::vector<std::vector<glm::vec3>> ai_waypoints, Audio::AudioEngine& audio,
		Render::MaterialHandle wallMaterialHandle, UiGame* UI, bool& menuActive, std::vector<int>& waypointOptions)
		: Bike(window, render, start_place, group, audio, wallMaterialHandle, UI, menuActive),
		  ai_waypoints(ai_waypoints), waypointOptions(waypointOptions) {
		engineAudio->changeGain(0.025);
		FRAGAudio->changeGain(0.15);
		JumpAudio->changeGain(0.075);
		StrafeAudio->changeGain(0.075);
		WADAudio->changeGain(0.125);
		FRAGImpactAudio->changeGain(0.15);
		chassisAudio->changeGain(0.1);
		SlipstreamingAudio->changeGain(0.5);
		
		srand((unsigned int)time(NULL));
		getNewLane(waypointOptions);
		//waypoints = ai_waypoints[8];

	};

	void update(float deltaTime) override {
		Bike::update(deltaTime);
		if (!getLocked()) {
			if (buffer < 0) {
				followWaypoint();
			} else {
				buffer--;
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
				//waypoints = ai_waypoints[4+getLap()];

				addLap();
				resetWaypoint();
				getNewLane(waypointOptions);
			} else {
				nextWaypoint++;
				//std::cout << currentWaypoint << std::endl;
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
		//if radius too high booster?

		if (dist > 25.0f) {//~20-90 big curve 140-185 little curve? 20-76 and 130-180
			if (d > 0) { //left
				bikeReleaseSteer(getId());
				bikeTurnPrecise(getId(), glm::min(3.5f * radius, 1.f));
				if (Slipstreaming) {//hardcoded estimates of where the curves are to slow down for
					if ((currentWaypoint > 30 && currentWaypoint < 85) ||
						(currentWaypoint > 135 && currentWaypoint < 190)) {
						bikeAcceleratePrecise(getId(), slowSlip);
					} else {
						bikeAcceleratePrecise(getId(), fastSlip);
					}
				} else {// hardcoded estimates of where the curves are to slow down for
					if ((currentWaypoint > 30 && currentWaypoint < 85) ||
						(currentWaypoint > 135 && currentWaypoint < 190)) { 
						bikeAcceleratePrecise(getId(), slow);
					} else {
						bikeAcceleratePrecise(getId(), fast);
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
			bikeReleaseAll(getId());
		}
	}

	void getNewLane(std::vector<int>& waypointOptions) {
		if (waypointOptions.size() > 0) {
			//std::cout << waypointOptions.size() << std::endl;
			int waypointChoice = rand() % waypointOptions.size();
			waypoints = ai_waypoints[waypointChoice];
			waypointOptions.erase(waypointOptions.begin() + waypointChoice);
			//for (auto& waypoint : waypointOptions) {
			//	std::cout << waypoint << " ";
			//}
			//std::cout << std::endl;
		}
	}
};