#pragma once

#include "car.hpp"

class BikeAI : public Bike {
  private:
  public:
	BikeAI(Render::Render& render, int start_place, Render::Group& group)
		: Bike(render, start_place, group){};
};