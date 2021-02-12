#pragma once

#include "game_object.hpp"

class Car : public GameObject {
  private:
	int lap = 1;
	int checkpoint = 0;
	int place = 0;

  public:
	// change assets/test.glb to car model path
	Car(Render::Render& render, int start_place, physx::PxTransform& pt)
		: GameObject(render, "assets/DamagedHelmet.glb", pt), place(start_place){};

	// not sure if this would go here
	void SpawnWall() {
		// spawn wall behind car
	}

	void setPlace(int n) { place = n; }
	int getPlace() { return place; }

	void addLap(int n) { lap++; }
	int getLap() { return lap; }

	void addCheckpoint() { checkpoint++; }
	void resetCheckpoint() { checkpoint = 0; }
	int getCheckpoint() { return checkpoint; }
};