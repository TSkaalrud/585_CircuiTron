#pragma once

#include "bike.hpp"

#include <iostream>

class BikeAI : public Bike {
  private:
	bool left = false;
	bool right = false;

	int buffer = 100;
  public:
	BikeAI(Render::Render& render, int start_place, physx::PxTransform& pt, Render::Group& group)
		: Bike(render, start_place, pt, group){};

	void update(float deltaTime) override {
		followPlayer();

		model->setTransform(convertTransform(getBikeTransform(1)) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
	}

	void followPlayer() { 
		physx::PxTransform player = getBikeTransform(0);
		physx::PxTransform ai = getBikeTransform(1);

		physx::PxVec3 v = ai.q.getBasisVector1()*1000 + ai.p;

		//std::cout << v.x << ", " << v.y << ", "
		//		  << v.z << std::endl;

		//std::cout << getAngleBetween(glm::vec3(ai.p.x, ai.p.y, ai.p.z), glm::vec3(player.p.x, player.p.y, player.p.z))
		//		  << std::endl;

		//std::cout << v.z - ai.p.z << std::endl;
		//std::cout << v.x - ai.p.x << std::endl;

		physx::PxVec3 pn = player.p;
		physx::PxVec3 an = ai.p;

		//std::cout << "R:" << v.x << ", " << v.z << std::endl;
		//std::cout << "P:" << pn.x << ", " << pn.z << std::endl;
		//std::cout << "A:" << an.x << ", " << an.z << std::endl;

		float d = (pn.x - an.x) * (v.z - an.z) - (pn.z - an.z) * (v.x - an.x);
		float dist = glm::sqrt(glm::pow(pn.x - an.x, 2) + glm::pow(pn.y - an.y, 2));

		if (d < 0) {
			std::cout << "left" << std::endl;
		} else if (d > 0) {
			std::cout << "right" << std::endl;
		} else {
			std::cout << "on" << std::endl;
		}
		
		if (dist > 25.0f) {
			if (d < 0) {
				//std::cout << "left" << std::endl;
				if (!left) {
					bikeReleaseSteer(1);
				}
				left = true;
				right = false;
				bikeTurnLeft(1);
				bikeAccelerate(1);
			} else if (d > 0) {
				//std::cout << "right" << std::endl;
				if (!right) {
					bikeReleaseSteer(1);
				}
				right = true;
				left = false;
				bikeTurnRight(1);
				bikeAccelerate(1);
			} else {
				std::cout << "on" << std::endl;
			}
		} else {
			bikeReleaseGas(1);
			left = false;
			right = false;
		}

		/*
		if (player.p.x > ai.p.x && left > 0) {
			bikeTurnLeft(1);
			bikeAccelerate(1);

			left--;
			right = 10;
		} else if (player.p.x < ai.p.x && right > 0) {
			std::cout << "yup" << std::endl;
			bikeTurnRight(1);
			bikeAccelerate(1);

			right--;
			left = 10;

		} else if (player.p.z - 50.0f > ai.p.z) {
			bikeReleaseSteer(1);
			bikeAccelerate(1);
			
		} else {
			bikeReleaseSteer(1);
			bikeReleaseGas(1);
		}
		*/
	}

	float getAngleBetween(glm::vec3 a, glm::vec3 b) { 
		glm::vec3 da = glm::normalize(a);
		glm::vec3 db = glm::normalize(b - a);
		return glm::acos(glm::dot(da, db));
	}
};