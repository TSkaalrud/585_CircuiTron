#pragma once

#include "game_object.hpp"

class Track : public GameObject {
  private:
  public:
	Track(Render::Render& render, physx::PxTransform& pt, Render::Group& group)
		: GameObject(render, pt, group){};

	void update(float deltaTime) override {}
};