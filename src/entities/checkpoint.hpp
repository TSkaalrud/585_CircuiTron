#pragma once

#include "game_object.hpp"

class Checkpoint : public GameObject {
	private:
	public:
		Checkpoint(Render::Render& render, physx::PxTransform& pt, Render::Group& group) : GameObject(render, pt, group){};

		void update(float deltaTime) override {}
};