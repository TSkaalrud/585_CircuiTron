#pragma once

#include "game_object.hpp"

class Bike : public GameObject {
  private:
	int lap = 1;
	int checkpoint = 0;
	int place = 0;
	int health = 100;

  public:
	Bike(Render::Render& render, int start_place, Render::Group& group)
		: GameObject(render, group), place(start_place){};

	void setPlace(int n) { place = n; }
	int getPlace() { return place; }

	void addLap() { lap++; }
	int getLap() { return lap; }

	void addCheckpoint() { checkpoint++; }
	void resetCheckpoint() { checkpoint = 0; }
	int getCheckpoint() { return checkpoint; }

	void loseHealth() { health -= 10; }

	void wallCollision() { 
		loseHealth();
		//play sound?
	}
};