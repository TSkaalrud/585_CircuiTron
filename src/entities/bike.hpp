#pragma once

#include "game_object.hpp"
#include "physics/physics.h"

class Bike : public GameObject {
  private:
	int lap = 1;
	int waypoint = 0;
	int place;
	int health = 100;
	int id;
	bool locked = false;

  public:
	Bike(Render::Render& render, int start_place, Render::Group& group)
		: GameObject(render, group), place(start_place), id(start_place-1){};

	int getId() { return id; }

	void lockBike() { 
		locked = true;
		bikeReleaseAll(id);
	}
	void unlockBike() { locked = false; }
	bool getLocked() { return locked; }

	void setPlace(int n) { place = n; }
	int getPlace() { return place; }

	void addLap() { lap++; }
	int getLap() { return lap; }

	void addWaypoint() { waypoint++; }
	void resetWaypoint() { waypoint = 0; }
	int getWaypoint() { return waypoint; }

	void loseHealth() { health -= 10; }

	void wallCollision() { 
		loseHealth();
		//play sound?
	}
};