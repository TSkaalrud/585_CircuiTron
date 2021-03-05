#pragma once

#include "game_object.hpp"

class Track : public GameObject {
  private:
  public:
	Track(Render::Render& render, Render::Group& group)
		: GameObject(render, group){};

	void update(float deltaTime) override {}
};