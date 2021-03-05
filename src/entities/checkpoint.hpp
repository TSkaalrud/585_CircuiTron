#pragma once

#include "game_object.hpp"

class Checkpoint : public GameObject {
	private:
	public:
		Checkpoint(Render::Render& render, Render::Group& group) : GameObject(render, group){};

		void update(float deltaTime) override {}
};