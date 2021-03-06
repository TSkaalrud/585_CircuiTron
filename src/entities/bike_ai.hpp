#pragma once

#include "bike.hpp"

#include <iostream>

class BikeAI : public Bike {
  private:
	//direction of the bike's current target
	bool left = false;
	bool right = false;
	//timer before AI bikes will start driving
	int buffer = 120;
	//AI waypoint list, current target, and next target
	std::vector<glm::vec3> waypoints;
	int currentWaypoint = 0, nextWaypoint = 1;


  public:
	BikeAI(
		Render::Render& render, int start_place, Render::Group& group, std::vector<glm::vec3> waypoints)
		: Bike(render, start_place, group), waypoints(waypoints) {};

	void update(float deltaTime) override {
		if (!getLocked()) {
			if (buffer < 0) {
				followWaypoint();
			}
			buffer--;
		}
		
		model->setTransform(convertTransform(getBikeTransform(1)) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
	}

	void followWaypoint() { 
		glm::vec3 target = waypoints[currentWaypoint];
		physx::PxTransform ai = getBikeTransform(1);

		physx::PxVec3 heading = ai.q.getBasisVector2() + ai.p;

		physx::PxVec3 ai_pos = ai.p;

		float d = (target.x - ai_pos.x) * (heading.z - ai_pos.z) - (target.z - ai_pos.z) * (heading.x - ai_pos.x);
		float dist = glm::sqrt(glm::pow(target.x - ai_pos.x, 2) + glm::pow(target.z - ai_pos.z, 2));
		
		//update to next WP if distance is less than 10 -- play with this number for feel
		if (dist < 10) {
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

		if (dist > 10.0f) {
			if (d > 0) {
				//std::cout << "left" << std::endl;

				bikeReleaseSteer(getId());
				bikeTurnPrecise(getId(), radius);
				//bikeTurnLeft(getId());
				bikeAcceleratePrecise(getId(), 0.5f);
			} else if (d < 0) {
				//std::cout << "right" << std::endl;

				bikeReleaseSteer(getId());
				bikeTurnPrecise(getId(), -radius);
				//bikeTurnRight(getId());
				bikeAcceleratePrecise(getId(), 0.5f);
			} else {
				//std::cout << "on" << std::endl;
				bikeReleaseSteer(getId());
			}
		} else {
			bikeReleaseAll(getId());
		}
	}
};