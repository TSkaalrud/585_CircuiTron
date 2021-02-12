#pragma once

#include "game_object.hpp"

class Checkpoint : public GameObject {
	private:
		Render::Render& render;
		
	public:
		Checkpoint(Render::Render& r) : GameObject(r, "path/to/model") {}

		// this is place holder till we add collisions
		void onCollisionWithCar(Car c) { c.addLap(); }
};