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
		//physx::PxTransform player = getBikeTransform(0);
		glm::vec3 target = waypoints[currentWaypoint];
		physx::PxTransform ai = getBikeTransform(1);

		physx::PxVec3 v = ai.q.getBasisVector2() + ai.p;

		//physx::PxVec3 pn = player.p;
		physx::PxVec3 an = ai.p;

		float d = (target.x - an.x) * (v.z - an.z) - (target.z - an.z) * (v.x - an.x);
		float dist = glm::sqrt(glm::pow(target.x - an.x, 2) + glm::pow(target.z - an.z, 2));
		
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


		if (dist > 10.0f) {
			if (d > 0) {
				//std::cout << "left" << std::endl;

				bikeReleaseSteer(getId());
				bikeTurnPrecise(getId(), 0.25f);
				//bikeTurnLeft(getId());
				bikeAccelerate(getId());
			} else if (d < 0) {
				//std::cout << "right" << std::endl;

				bikeReleaseSteer(getId());
				bikeTurnPrecise(getId(), -0.25f);
				//bikeTurnRight(getId());
				bikeAccelerate(getId());
			} else {
				//std::cout << "on" << std::endl;
				bikeReleaseSteer(getId());
			}
		} else {
			bikeReleaseAll(getId());
		}
	}
};