#pragma once

#include "game_object.hpp"

class Bike : public GameObject {
  private:
	int lap = 1;
	int checkpoint = 0;
	int place = 0;

  public:
	Bike(Render::Render& render, int start_place, physx::PxTransform& pt, Render::Group& group)
		: GameObject(render, pt, group), place(start_place){};

	void setPlace(int n) { place = n; }
	int getPlace() { return place; }

	void addLap() { lap++; }
	int getLap() { return lap; }

	void addCheckpoint() { checkpoint++; }
	void resetCheckpoint() { checkpoint = 0; }
	int getCheckpoint() { return checkpoint; }
};