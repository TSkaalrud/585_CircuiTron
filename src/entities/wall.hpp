#pragma once

#include "game_object.hpp"

class Wall : public GameObject {
  private:

  public:
	Wall(Render::Render& render, physx::PxTransform& pt, Render::Group& group)
		: GameObject(render, pt, group) {};
};