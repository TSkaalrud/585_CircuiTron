#pragma once

#include "bike.hpp"

#include <iostream>

class BikeAI : public Bike {
  private:
	//direction of the bike's current target
	bool left = false;
	bool right = false;
	//timer before AI bikes will start driving
	int buffer = 60;
	//AI waypoint list, current target, and next target
	std::vector<glm::vec3> waypoints;
	int currentWaypoint = 0, nextWaypoint = 1;


  public:
	BikeAI(
		Render::Render& render, int start_place, Render::Group& group, std::vector<glm::vec3> waypoints,
		Audio::AudioEngine& audio, WallManager* wm)
		: Bike(render, start_place, group, audio, wm), waypoints(waypoints) {};

	void update(float deltaTime) override {
		if (!getLocked()) {
			if (buffer < 0) {
				followWaypoint();
			}
			buffer--;
		}
		
		model->setTransform(convertTransform(getBikeTransform(getId())) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));

		spawnWall(deltaTime, getId());
	}

	void followWaypoint() { 
		glm::vec3 target = waypoints[currentWaypoint];
		physx::PxTransform ai = getBikeTransform(getId());

		physx::PxVec3 heading = ai.q.getBasisVector2() + ai.p;

		physx::PxVec3 ai_pos = ai.p;

		float d = (target.x - ai_pos.x) * (heading.z - ai_pos.z) - (target.z - ai_pos.z) * (heading.x - ai_pos.x);
		float dist = glm::sqrt(glm::pow(target.x - ai_pos.x, 2) + glm::pow(target.z - ai_pos.z, 2));
		
		//update to next WP if distance is less than 10 -- play with this number for feel
		if (dist < 25) {
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
		//std::cout << currentWaypoint << " distance = " << (int)dist << std::endl;

		physx::PxTransform player = getBikeTransform(0);
		physx::PxVec3 player_pos = player.p;

		glm::vec3 toTarget(target.x - ai_pos.x, target.y - ai_pos.y, target.z - ai_pos.z);
		glm::vec3 toHeading(heading.x - ai_pos.x, heading.y - ai_pos.y, heading.z - ai_pos.z);

		float dot = glm::dot(toTarget, toHeading);
		float mag = glm::length(toTarget) * glm::length(toHeading);

		float angle = glm::acos(dot / mag);

		//std::cout << angle << std::endl;

		float angleRange = (3.14f - 0.0f);
		float radiusRange = (1.0f - 0.0f);

		float radius = (((angle - 0.0f) * radiusRange) / angleRange) - 0.0f;
		//std::cout << radius << std::endl;


		if (dist > 25.0f) {
			if (d > 0) {
				//std::cout << "left" << std::endl;

				bikeReleaseSteer(getId());
				bikeTurnPrecise(getId(), glm::min(2*radius,1.f));
				//bikeTurnLeft(getId());
				bikeAcceleratePrecise(getId(), 0.75f);
			} else if (d < 0) {
				//std::cout << "right" << std::endl;

				bikeReleaseSteer(getId());
				bikeTurnPrecise(getId(), glm::max(-2*radius,-1.f));
				//bikeTurnRight(getId());
				bikeAcceleratePrecise(getId(), 0.75f);
			} else {
				//std::cout << "on" << std::endl;
				bikeReleaseSteer(getId());
			}
		} else {
			bikeReleaseAll(getId());
		}
	}
};