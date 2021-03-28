#pragma once

#include "bike.hpp"

#include <iostream>

class BikeAI : public Bike {
  private:
	// direction of the bike's current target
	bool left = false;
	bool right = false;
	// timer before AI bikes will start driving
	int buffer = 60;

	std::vector<std::vector<glm::vec3>> ai_waypoints; // a list of waypoints for each ai bike
	int currentWaypoint = 0, nextWaypoint = 1;

  public:
	BikeAI(
		Render::Render& render, int start_place, Render::Group& group, std::vector<std::vector<glm::vec3>> ai_waypoints,
		Audio::AudioEngine& audio, Render::MaterialHandle wallMaterialHandle)
		: Bike(render, start_place, group, audio, wallMaterialHandle), ai_waypoints(ai_waypoints) {
		waypoints = ai_waypoints[getId() - 1];
	};

	void update(float deltaTime) override {
		Bike::update(deltaTime);
		if (!getLocked()) {
			if (buffer < 0) {
				followWaypoint();
			}
			buffer--;
		}
		if (getBikeTransform(getId()).p.y < 0) {
			resetBike();
		}
	}

	void followWaypoint() {
		glm::vec3 target = waypoints[currentWaypoint];
		physx::PxTransform ai = getBikeTransform(getId());

		physx::PxVec3 heading = ai.q.getBasisVector2() + ai.p;

		physx::PxVec3 ai_pos = ai.p;

		float d = (target.x - ai_pos.x) * (heading.z - ai_pos.z) - (target.z - ai_pos.z) * (heading.x - ai_pos.x);
		float dist = glm::sqrt(glm::pow(target.x - ai_pos.x, 2) + glm::pow(target.z - ai_pos.z, 2));

		// update to next WP if distance is less than 10 -- play with this number for feel
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
		// std::cout << currentWaypoint << " distance = " << (int)dist << std::endl;

		physx::PxTransform player = getBikeTransform(0);
		physx::PxVec3 player_pos = player.p;

		glm::vec3 toTarget(target.x - ai_pos.x, target.y - ai_pos.y, target.z - ai_pos.z);
		glm::vec3 toHeading(heading.x - ai_pos.x, heading.y - ai_pos.y, heading.z - ai_pos.z);

		float dot = glm::dot(toTarget, toHeading);
		float mag = glm::length(toTarget) * glm::length(toHeading);

		float angle = glm::acos(dot / mag);

		// std::cout << angle << std::endl;

		float angleRange = (3.14f - 0.0f);
		float radiusRange = (1.0f - 0.0f);

		float radius = (((angle - 0.0f) * radiusRange) / angleRange) - 0.0f;
		// std::cout << radius << std::endl;

		if (dist > 25.0f) {
			if (d > 0) {
				// std::cout << "left" << std::endl;

				bikeReleaseSteer(getId());
				bikeTurnPrecise(getId(), glm::min(2 * radius, 1.f));
				// bikeTurnLeft(getId());
				bikeAcceleratePrecise(getId(), 0.825f);
			} else if (d < 0) {
				// std::cout << "right" << std::endl;

				bikeReleaseSteer(getId());
				bikeTurnPrecise(getId(), glm::max(-2 * radius, -1.f));
				// bikeTurnRight(getId());
				bikeAcceleratePrecise(getId(), 0.825f);
			} else {
				// std::cout << "on" << std::endl;
				bikeReleaseSteer(getId());
			}
		} else {
			bikeReleaseAll(getId());
		}
	}

	void resetBike() {
		physx::PxTransform resetLocation = getBikeTransform(getId());
		int waypoint = 0;
		int waypointOffset = 5;
		// waypointOffset is used to manage the fact that we hit waypoints within a certain radius of us.
		if (currentWaypoint < waypointOffset) {
			waypoint = waypoints.size() - (waypointOffset - currentWaypoint);
		} else {
			waypoint = currentWaypoint - waypointOffset;
		}

		resetLocation.p.x = waypoints[waypoint].x;
		resetLocation.p.y = waypoints[waypoint].y + 5; // drop me in to avoid clipping
		resetLocation.p.z = waypoints[waypoint].z;
		float rads;
		physx::PxVec3 axis;
		resetLocation.q.toRadiansAndUnitAxis(rads, axis);
		resetLocation.q.x = 0;
		resetLocation.q.y = sin(rads / 2);
		resetLocation.q.z = 0;
		resetLocation.q.w = cos(rads / 2);
		resetBikePos(getId(), resetLocation);
	}
};