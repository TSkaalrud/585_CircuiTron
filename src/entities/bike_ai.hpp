#pragma once

#include "bike.hpp"

#include <iostream>

class BikeAI : public Bike {
  private:
	bool left = false;
	bool right = false;

	int buffer = 100;
	int buffer2 = 100;
  public:
	BikeAI(Render::Render& render, int start_place, physx::PxTransform& pt, Render::Group& group)
		: Bike(render, start_place, pt, group){};

	void update(float deltaTime) override {
		if (buffer < 0) {
			followPlayer();
		}
		buffer--;

		model->setTransform(convertTransform(getBikeTransform(1)) * glm::scale(glm::mat4(1.0f), glm::vec3(2.0f)));
	}

	void followPlayer() { 
		physx::PxTransform player = getBikeTransform(0);
		physx::PxTransform ai = getBikeTransform(1);

		physx::PxVec3 v = ai.q.getBasisVector2() + ai.p;

		physx::PxVec3 pn = player.p;
		physx::PxVec3 an = ai.p;

		float d = (pn.x - an.x) * (v.z - an.z) - (pn.z - an.z) * (v.x - an.x);
		float dist = glm::sqrt(glm::pow(pn.x - an.x, 2) + glm::pow(pn.z - an.z, 2));
		
		if (dist > 50.0f) {
			if (d > 0) {
				std::cout << "left" << std::endl;

				bikeReleaseSteer(1);
				bikeTurnPrecise(1, 0.25);
				bikeAccelerate(1);
			} else if (d < 0) {
				std::cout << "right" << std::endl;

				bikeReleaseSteer(1);
				bikeTurnPrecise(1, -0.25);
				bikeAccelerate(1);
			} else {
				std::cout << "on" << std::endl;
				bikeReleaseSteer(1);
			}
		} else {
			bikeReleaseAll(1);
		}
		


	}
};