#pragma once

#include "car.hpp"

class CarAI : public Car {
  private:
  public:
	CarAI(Render::Render& render, int start_place, physx::PxTransform& pt, Render::Group& group)
		: Car(render, start_place, pt, group){};
};